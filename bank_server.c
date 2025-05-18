#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit
#include <string.h>     // strcmp, strlen, snprintf
#include <unistd.h>     // fork, close, read, write
#include <sys/types.h>  // socket types
#include <sys/socket.h> // socket, bind, accept
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // htons, INADDR_ANY

#define PORT 3333  // TCP port number
#define BACKLOG 10 // max pending connections
#define BUF_SZ 256 // buffer size for commands

#include "bankapp.h" // your network wrappers

// Send a line (adds '\n')
void send_line(int sock, const char *msg)
{
    write(sock, msg, strlen(msg));
    write(sock, "\n", 1);
}

// Read up to BUF_SZ−1 chars until '\n'; returns bytes read
ssize_t recv_line(int sock, char *buf)
{
    ssize_t i = 0;
    while (i < BUF_SZ - 1)
    {
        char c;
        if (read(sock, &c, 1) <= 0)
            return 0;
        if (c == '\n')
            break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

// Handle one client: loop reading commands → executing → replying
void handle_client(int client_fd)
{
    char buf[BUF_SZ];
    while (recv_line(client_fd, buf))
    {
        char cmd[16];
        sscanf(buf, "%15s", cmd);

        if (strcmp(cmd, "OPEN") == 0)
        {
            char name[50], nid[20], type[10];
            sscanf(buf + 5, "%49s %19s %9s", name, nid, type);
            int acct_no, pin;
            open_account_network(name, nid, type, &acct_no, &pin);
            char resp[64];
            snprintf(resp, sizeof resp, "OK %d %d", acct_no, pin);
            send_line(client_fd, resp);
        }
        else if (strcmp(cmd, "DEPOSIT") == 0)
        {
            int an, p, amt;
            sscanf(buf + 8, "%d %d %d", &an, &p, &amt);
            int bal = deposit_network(an, p, amt);
            if (bal >= 0)
            {
                char resp[64];
                snprintf(resp, sizeof resp, "OK %d", bal);
                send_line(client_fd, resp);
            }
            else
            {
                send_line(client_fd, "ERR deposit failed");
            }
        }
        // … (similarly handle WITHDRAW, BALANCE, STATEMENT, CLOSE) …
        else if (strcmp(cmd, "QUIT") == 0)
        {
            break;
        }
        else
        {
            send_line(client_fd, "ERR unknown command");
        }
    }
    close(client_fd);
    exit(0); // child process must exit
}

int main()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY};
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof addr) < 0)
    {
        perror("bind");
        exit(1);
    }
    listen(listen_fd, BACKLOG);
    printf("Listening on port %d...\n", PORT);

    while (1)
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int client_fd = accept(listen_fd, (struct sockaddr *)&cli, &len);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            close(client_fd);
        }
        else if (pid == 0)
        {
            close(listen_fd);         // child closes listening socket
            handle_client(client_fd); // never returns
        }
        else
        {
            close(client_fd); // parent closes client socket
            // optional: reap children via SIGCHLD
        }
    }
    return 0;
}
