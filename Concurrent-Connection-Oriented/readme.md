# Concurrent Connection‐Oriented Bank Server

This repository contains three variants of a concurrent, connection‐oriented TCP bank‐server implemented in C:

1. **Process‐based** (`bank_server_process.c`)  
2. **Thread‐based** (`bank_server_threaded.c`)  
3. **Asynchronous I/O** using `select()` (`bank_server_async.c`)  

A simple iterative client (`bank_client.c`) is also provided for testing and demonstration.

---

## Features

- **OPEN**: Create an account (minimum Ksh 1,000), returns account number & PIN  
- **DEPOSIT**: Add funds (min Ksh 500)  
- **WITHDRAW**: Remove funds (in 500 increments, leaving ≥ Ksh 1,000)  
- **BALANCE**: Query current balance  
- **STATEMENT**: Retrieve last five transactions  
- **CLOSE**: Close account  

All servers dispatch incoming client connections concurrently while preserving the “connection‐oriented” TCP model.

---

## Prerequisites

- GCC (or compatible C compiler)  
- POSIX‐compliant OS (Linux, macOS, BSD)  
- `make` (optional, for convenience)  

---

## Build Instructions

From the project root, run:

```bash
# Process‐based server
gcc -I. -o bank_server_process \
    bank_server_process.c \
    bankapp.c \
    bankapp_network.c \
    command_processor.c \
    -lm

# Thread‐based server
gcc -I. -o bank_server_threaded \
    bank_server_threaded.c \
    bankapp.c \
    bankapp_network.c \
    command_processor.c \
    -lpthread

# Asynchronous I/O server
gcc -I. -o bank_server_async \
    bank_server_async.c \
    bankapp.c \
    bankapp_network.c \
    command_processor.c

# Iterative client
gcc -o bank_client bank_client.c
```

Note: `-I.` tells the compiler to look in the current directory for header files.

## Running the Servers
Open three separate terminals (or background processes):

```bash
# Process‐based
./bank_server_process

# Thread‐based
./bank_server_threaded

# Async I/O
./bank_server_async
```

Each will listen on port 3333 by default.

## Client Usage
In another terminal, connect with the supplied client:

```bash
./bank_client
```

Type any of the following commands (one per line):
```php-template
OPEN <Name> <NationalID> <savings|checking>
DEPOSIT <AccountNo> <PIN> <Amount>
WITHDRAW <AccountNo> <PIN> <Amount>
BALANCE <AccountNo> <PIN>
STATEMENT <AccountNo> <PIN>
CLOSE <AccountNo> <PIN>
QUIT
```

The server will respond with either OK … or ERR … messages.

## Sample Session
```yaml
> OPEN Alice 12345678 savings
OK 1001 4321

> DEPOSIT 1001 4321 2000
OK 3000

> WITHDRAW 1001 4321 500
OK 2500

> BALANCE 1001 4321
OK 2500

> STATEMENT 1001 4321
2025-06-08 11:02:12 OPEN +1000
2025-06-08 11:04:05 DEPOSIT +2000
2025-06-08 11:06:30 WITHDRAW -500

> CLOSE 1001 4321
OK

> QUIT
```

## Project Structure
```bash
.
├── bank_server_process.c     # Process‐forking variant
├── bank_server_threaded.c    # POSIX‐threads variant
├── bank_server_async.c       # select()‐based async I/O variant
├── bank_client.c             # Iterative command‐line client
├── bankapp.c                 # Core banking logic
├── bankapp_network.c         # Network‐specific wrappers (open/deposit/etc.)
├── bankapp.h                 # Shared declarations
├── command_processor.c       # Parses client commands & invokes network API
└── command_processor.h       # Prototype for process_command()

..
├── Concurrent Connection-Oriented Server Conceptual Algorithm    # Docx and Pdf files
```

## Notes
Feel free to adjust PORT, buffer sizes, or logging as needed in the source.

Error handling is basic; in production you’d add better validation, logging, and resource cleanup.

The async‐I/O variant can scale to more connections without per‐connection threads/processes, but you must handle partial reads/writes carefully.

The Repo includes the compiled versions which one can run right after cloning the repository

## License
Distributed under the MIT License. See LICENSE for details.

## 📬 Contact
Website: https://david-okello.webflow.io/

LinkedIn: https://www.linkedin.com/in/david-okello-3599b51a0/

Email: okellodavid002@gmail.com

