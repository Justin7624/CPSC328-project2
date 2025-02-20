# CPSC328-project2
# Number Guessing Game using Pipes and Fork

## Overview
This project implements a number guessing game using **Inter-Process Communication (IPC)** with `fork()` and `pipe()`. The program consists of a **parent process** and a **child process** that communicate via pipes. The parent selects a **secret number**, while the child **guesses** the number using **binary search**. The parent provides feedback until the correct number is found.

## Features
- Parent and child communication via **pipes**.
- The **parent process** selects the secret number and responds to guesses.
- The **child process** performs a **binary search** to guess the number.
- Proper error handling for **invalid inputs** and **pipe failures**.
- Uses **C++** for implementation.

## Usage
### Compilation
To compile the project, use the provided **Makefile**:
```bash
make
```
This will generate an executable named `p2`.

### Running the Program
Run the program with the following command-line arguments:
```bash
./p2 -s <secret> -l <lower_bound> -u <upper_bound>
```
#### Example:
```bash
./p2 -s 42 -l 1 -u 100
```
This will start the game with **42** as the secret number, within the range **1 to 100**.

### Expected Output
1. The **child process** receives the range and starts guessing using **binary search**.
2. The **parent process** provides feedback (`Too low`, `Too high`, or `Correct!`).
3. The **game ends** when the child correctly guesses the number.

### Cleaning Up
To remove compiled files:
```bash
make clean
```

## Implementation Details
### Parent Process:
- Sends the **lower** and **upper bounds** to the child.
- Receives the child's **guess** and responds:
  - `-1` if the guess is **too low**.
  - `1` if the guess is **too high**.
  - `0` if the guess is **correct**.
- Waits for the child to **terminate** and exits.

### Child Process:
- Reads the **lower** and **upper bounds**.
- Uses **binary search** to guess the number.
- Sends each guess to the parent.
- Adjusts bounds based on parent's response.
- Terminates upon correct guess.

## Code Structure
- `p2.cpp` - Main program implementing parent-child communication.
- `Makefile` - Compilation and cleanup rules.

## Error Handling
- Checks for **invalid input values**.
- Ensures **valid bounds** (lower ≤ secret ≤ upper).
- Handles **pipe failures** and **fork errors**.

## Author
- **Justin Allen**
