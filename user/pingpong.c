#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pipe1[2]; // Parent to child
    int pipe2[2]; // Child to parent
    char buf[1];
    int pid;

    // Create pipes
    pipe(pipe1);
    pipe(pipe2);

    if ((pid = fork()) == 0) {
        // Child process
        close(pipe1[1]); // Close unused write end of pipe1
        close(pipe2[0]); // Close unused read end of pipe2

        read(pipe1[0], buf, 1); // Read from parent
        printf("%d: received ping\n", getpid());

        write(pipe2[1], buf, 1); // Send back to parent
        close(pipe1[0]);
        close(pipe2[1]);

        exit(0);
    } else {
        // Parent process
        close(pipe1[0]); // Close unused read end of pipe1
        close(pipe2[1]); // Close unused write end of pipe2

        buf[0] = 'A'; // Send a byte to the child
        write(pipe1[1], buf, 1);

        read(pipe2[0], buf, 1); // Read back from child
        printf("%d: received pong\n", getpid());

        close(pipe1[1]);
        close(pipe2[0]);

        wait(0); // Wait for child to finish
    }

    exit(0);
}