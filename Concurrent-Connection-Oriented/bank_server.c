#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // fork(), close(), read(), write()
#include <sys/types.h>
#include <sys/socket.h>     // socket(), bind(), listen(), accept()
#include <netinet/in.h>     // sockaddr_in, htons(), INADDR_ANY
#include <arpa/inet.h>      // inet_ntoa()
#include <signal.h>         // signal(), SIG_IGN

#define PORT     3333
#define BACKLOG  10
#define BUF_SZ   256

#include "bankapp.h"

//
// Send a null‑terminated string plus “\n” over sock_fd
//
void send_line(int sock_fd, const char *msg) {
    write(sock_fd, msg, strlen(msg));
    write(sock_fd, "\n", 1);
}

//
// Read one line (up to BUF_SZ‑1 chars) from sock_fd into buf.
// Stops at '\n'. Returns number of bytes read (excluding '\n'), or 0 on EOF.
//
ssize_t recv_line(int sock_fd, char *buf) {
    ssize_t total = 0;
    while (total < BUF_SZ - 1) {
        char c;
        ssize_t n = read(sock_fd, &c, 1);
        if (n <= 0) return 0;  // client closed or error
        if (c == '\n') break;
        buf[total++] = c;
    }
    buf[total] = '\0';
    return total;
}

//
// Handle one connected client. Loop: recv command line, parse, call network wrappers,
// send back “OK …” or “ERR …”, until client sends “QUIT”.
//
void handle_client(int client_fd) {
    char buf[BUF_SZ];
    while (1) {
        ssize_t len = recv_line(client_fd, buf);
        if (len == 0) break;  // client closed

        char cmd[16];
        sscanf(buf, "%15s", cmd);

        // OPEN name nid acct_type
        if (strcmp(cmd, "OPEN") == 0) {
            char name[50], nid[20], type[10];
            sscanf(buf + 5, "%49s %19s %9s", name, nid, type);
            int acct_no, pin;
            open_account_network(name, nid, type, &acct_no, &pin);
            if (acct_no < 0) {
                send_line(client_fd, "ERR cannot open account");
            } else {
                char resp[64];
                snprintf(resp, sizeof(resp), "OK %d %d", acct_no, pin);
                send_line(client_fd, resp);
            }
        }
        // DEPOSIT acct_no PIN amount
        else if (strcmp(cmd, "DEPOSIT") == 0) {
            int an, p, amt;
            sscanf(buf + 8, "%d %d %d", &an, &p, &amt);
            int new_bal = deposit_network(an, p, amt);
            if (new_bal >= 0) {
                char resp[64];
                snprintf(resp, sizeof(resp), "OK %d", new_bal);
                send_line(client_fd, resp);
            } else {
                send_line(client_fd, "ERR deposit failed");
            }
        }
        // WITHDRAW acct_no PIN amount
        else if (strcmp(cmd, "WITHDRAW") == 0) {
            int an, p, amt;
            sscanf(buf + 9, "%d %d %d", &an, &p, &amt);
            int new_bal = withdraw_network(an, p, amt);
            if (new_bal >= 0) {
                char resp[64];
                snprintf(resp, sizeof(resp), "OK %d", new_bal);
                send_line(client_fd, resp);
            } else {
                send_line(client_fd, "ERR withdrawal failed");
            }
        }
        // BALANCE acct_no PIN
        else if (strcmp(cmd, "BALANCE") == 0) {
            int an, p;
            sscanf(buf + 8, "%d %d", &an, &p);
            int bal = balance_network(an, p);
            if (bal >= 0) {
                char resp[64];
                snprintf(resp, sizeof(resp), "OK %d", bal);
                send_line(client_fd, resp);
            } else {
                send_line(client_fd, "ERR balance check failed");
            }
        }
        // STATEMENT acct_no PIN
        else if (strcmp(cmd, "STATEMENT") == 0) {
            int an, p;
            sscanf(buf + 10, "%d %d", &an, &p);
            char *stm = statement_network(an, p);
            if (!stm) {
                send_line(client_fd, "ERR cannot get statement");
            } else {
                send_line(client_fd, "OK");
                write(client_fd, stm, strlen(stm));
                free(stm);
            }
        }
        // CLOSE acct_no PIN
        else if (strcmp(cmd, "CLOSE") == 0) {
            int an, p;
            sscanf(buf + 6, "%d %d", &an, &p);
            int r = close_account_network(an, p);
            if (r == 0) {
                send_line(client_fd, "OK");
            } else {
                send_line(client_fd, "ERR close failed");
            }
        }
        // QUIT
        else if (strcmp(cmd, "QUIT") == 0) {
            break;
        }
        // Unknown command
        else {
            send_line(client_fd, "ERR unknown command");
        }
    }
    close(client_fd);
    exit(0);  // child must exit
}

int main() {
    int listen_fd;
    struct sockaddr_in server_addr;

    // (a) Create TCP socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // (b) Allow quick reuse of address/port if server restarts
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    // (c) Bind to port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // (d) Listen
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Server listening on port %d …\n", PORT);

    // (e) Ignore SIGCHLD so that child processes are reaped automatically
    signal(SIGCHLD, SIG_IGN);

    // (f) Main accept() loop
    while (1) {
        int client_fd;
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
        }
        else if (pid == 0) {
            // Child
            close(listen_fd);
            handle_client(client_fd);
        }
        else {
            // Parent
            close(client_fd);
        }
    }

    return 0;
}
