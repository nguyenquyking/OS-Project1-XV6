#include "kernel/types.h"
#include "user/user.h"

// Declare primes() with __attribute__((noreturn))
void primes(int p_left) __attribute__((noreturn));

void primes(int p_left) {
    int num;
    
    // Read the first number from the left pipe (this is the prime for this process)
    if (read(p_left, &num, sizeof(num)) == 0) {
        close(p_left);  // No more numbers, close pipe and exit
        exit(0);
    }
    printf("prime %d\n", num);

    // Create a new pipe for this process to pass numbers to the next
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // Child process: Recursively handle the next prime
        close(p_left);
        close(p[1]);   // Close the write end of the new pipe
        primes(p[0]);  // Pass the read end to the next process
    } else {
        // Parent process: Filter numbers and pass to the child
        close(p[0]);   // Close the read end of the new pipe

        int next;
        while (read(p_left, &next, sizeof(next)) > 0) {
            if (next % num != 0) {
                write(p[1], &next, sizeof(next));  // Pass non-multiples to child
            }
        }
        close(p_left);  // Close the left pipe
        close(p[1]);    // Close the write end of the new pipe
        wait(0);        // Wait for the child process to finish
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // Child process: Start the prime sieve
        close(p[1]);  // Close the write end of the pipe
        primes(p[0]); // Start processing numbers
    } else {
        // Parent process: Feed numbers into the pipeline
        close(p[0]);  // Close the read end of the pipe
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i));
        }
        close(p[1]);  // Close the write end after feeding all numbers
        wait(0);      // Wait for the child process to complete
        exit(0);
    }
}