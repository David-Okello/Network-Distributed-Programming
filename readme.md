# Network Distributed Bank App

This repository contains a **Bank App** implemented in pure C as part of our Programming Assignment. The application provides basic banking operations through a console-based interface, demonstrating knowledge of data structures, memory management, and fundamental C programming concepts.

Note: My role was to implement the Concurrent Connection oriented (Processes, threads, asynchronous I/O). This repository mainly focuses on that, although I'll still upload my teammates versions of their implementations with time

## Assignment Overview

> As part of a group project, we are tasked with building a **bank application (V1)** and are required to implement a client server version of the program with client and server running on different machines. We are expected to provide the following variations of the client server program:

    1. Iterative Connectionless
    2. Iterative Connection Oriented
    3. Concurrent Connectionless (Processes, Threads, Asynchronous I/O (Non-Blocking))
    4. Concurrent Connection Oriented (Processes, Threads, Asynchronous I/O (Non-Blocking))

Before diving into the concurrent aspects, we first needed a working single-threaded version in C. One of our teammates initially provided a C++ implementation (`main.cpp`), so this repository contains the converted, pure C version (`bankapp.c`).


## Application Features (V1)

- **Open Account**: Create a new account with a unique account number and randomly generated 4-digit PIN, seeded for randomness.
- **Close Account**: Authenticate and remove an account, returning the final balance.
- **Withdraw / Deposit**: Enforce minimum transaction rules and ensure minimum balance is maintained.
- **Check Balance**: Display current balance for authenticated users.
- **Mini Statement**: Show the last 5 transactions (credits/debits) in FIFO order.

## File Structure

```
/ (root)
├── Concurrent Connection oriented (Processes, threads, asynchronous I/O)   # Folder contanining implementation, readme and conceptual server algorithm 
├── bankapp.c          # C source code for the bank app
└── README.md          # This file
```

## How to Build & Run

1. **Compile:**
   ```bash
   gcc -std=c11 -Wall -Wextra -o bank_app main.c
   ```
2. **Run:**
   ```bash
   ./bank_app    # On Unix/Linux/Mac
   bank_app.exe  # On Windows
   ```

---

Once this single-threaded C version is verified, the next step is to integrate concurrent connection handling (using processes, threads, or asynchronous I/O) as specified in the assignment.
