/***************************************************************
Author:         Justin Allen
Major:          Computer Science
Creation Date:  December 28, 2024
Due Date:       TBA
Course:         CPSC 328
Professor:      Schwesinger
Assignment:     #2
Filename:       p2.cpp
Purpose:        
    This program implements a number guessing game that utilizes fork() and pipe() to
    facilitate communication between a parent and child process. The parent process
    holds the secret number and guides the child process through binary search until
    the correct number is guessed. This project demonstrates inter-process communication
    (IPC) through pipe() calls and process management using fork().
***************************************************************/

#include <iostream>  // For standard I/O operations
#include <unistd.h>  // For fork(), pipe(), read(), write(), and close()
#include <sys/wait.h>  // For wait() to synchronize parent and child processes
#include <cstdlib>  // For atoi() to convert command-line arguments to integers

/***************************************************************
Function:       parentProcess
Description:    Manages the parent process, sends bounds to the child, and handles 
                feedback on guesses until the child guesses correctly.

Parameters:     int writePipe - write end of pipe to send data to child.
                int readPipe  - read end of pipe to receive guesses from child.
                int secret    - the secret number to be guessed.
                int lower     - the lower bound of the guessing range.
                int upper     - the upper bound of the guessing range.

Return Value:   None

Citations:
- Pipe communication: https://man7.org/linux/man-pages/man2/pipe.2.html
- Writing to pipes: https://man7.org/linux/man-pages/man2/write.2.html
- Reading from pipes: https://man7.org/linux/man-pages/man2/read.2.html
- Process synchronization: https://man7.org/linux/man-pages/man2/wait.2.html
***************************************************************/
void parentProcess(int writePipe, int readPipe, int secret, int lower, int upper) {

    // Send lower and upper bounds to child
    write(writePipe, &lower, sizeof(lower));  // Write lower bound to pipe
    write(writePipe, &upper, sizeof(upper));  // Write upper bound to pipe

    int guess, response;
    while (true) {
        if (read(readPipe, &guess, sizeof(guess)) <= 0) {  // Read guess from child
            std::cerr << "Failed to read from child or child exited. Exiting loop.\n";
            break;
        }

        std::cout << "Parent received guess: " << guess << std::endl;
        
        if (guess < secret) {
            response = -1;  // Guess is too low
        } else if (guess > secret) {
            response = 1;  // Guess is too high
        } else {
            response = 0;  // Guess is correct
        }

        if (write(writePipe, &response, sizeof(response)) <= 0) {  // Send response to child
            std::cerr << "Failed to write to child. Exiting.\n";
            break;
        }

        if (response == 0) break;  // Exit loop if correct guess
    }

    close(writePipe);  // Close write end of the pipe
    wait(nullptr);  // Wait for child to terminate
}

/***************************************************************
Function:       childProcess
Description:    Handles the child process, performing binary search guesses based
                on bounds received from the parent process.

Parameters:     int writePipe - write end of pipe to send guesses to parent.
                int readPipe  - read end of pipe to receive bounds and feedback.

Return Value:   None

Citations:
- Pipe communication: https://man7.org/linux/man-pages/man2/pipe.2.html
- Writing to pipes: https://man7.org/linux/man-pages/man2/write.2.html
- Reading from pipes: https://man7.org/linux/man-pages/man2/read.2.html
- Process termination: https://man7.org/linux/man-pages/man3/exit.3.html
***************************************************************/
void childProcess(int writePipe, int readPipe) {

    int lower, upper;
    read(readPipe, &lower, sizeof(lower));  // Read lower bound from parent
    read(readPipe, &upper, sizeof(upper));  // Read upper bound from parent

    std::cout << "Child received lower: " << lower << ", upper: " << upper << std::endl;

    int guess, response;
    while (lower <= upper) {
        guess = (lower + upper) / 2;  // Perform binary search midpoint guess
        std::cout << "Child guesses: " << guess << std::endl;
        write(writePipe, &guess, sizeof(guess));  // Send guess to parent
        
        if (read(readPipe, &response, sizeof(response)) <= 0) {  // Receive feedback
            std::cerr << "Failed to receive response. Exiting...\n";
            break;
        }

        if (response == -1) {
            std::cout << "Too low" << std::endl;
            lower = guess + 1;  // Adjust lower bound
        } else if (response == 1) {
            std::cout << "Too high" << std::endl;
            upper = guess - 1;  // Adjust upper bound
        } else {
            std::cout << "Correct!" << std::endl;
            break;
        }
    }

    close(readPipe);  // Close read end of the pipe
    exit(0);  // Terminate child process
}

/***************************************************************
Function:       main
Description:    Main function to parse arguments, create pipes, fork the process,
                and manage parent-child communication.

Parameters:     int argc - argument count.
                char* argv[] - array of command-line arguments.

Return Value:   int - returns 0 upon successful execution.

Citations:
- Argument parsing: https://cplusplus.com/reference/cstdlib/atoi/
- Pipe creation: https://man7.org/linux/man-pages/man2/pipe.2.html
- Process forking: https://man7.org/linux/man-pages/man2/fork.2.html
- Parent-child process communication: https://man7.org/linux/man-pages/man2/wait.2.html
***************************************************************/
int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " -s <secret> -l <lower> -u <upper>" << std::endl;
        return 1;
    }

    int secret = 0, lower = 0, upper = 0;
    
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]) == "-s") secret = std::atoi(argv[i + 1]);
        if (std::string(argv[i]) == "-l") lower = std::atoi(argv[i + 1]);
        if (std::string(argv[i]) == "-u") upper = std::atoi(argv[i + 1]);
    }

    if (lower > upper || secret < lower || secret > upper) {
        std::cerr << "Error: Invalid range or secret number outside of bounds." << std::endl;
        return 1;
    }

    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed.\n";
        return 1;
    }

    if (pid == 0) {
        close(pipe2[0]);
        close(pipe1[1]);
        childProcess(pipe2[1], pipe1[0]);  
    } else {
        close(pipe2[1]);
        close(pipe1[0]);
        parentProcess(pipe1[1], pipe2[0], secret, lower, upper);  
    }

    return 0;
}
