#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>          // read(), write(), close()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>       // inet_pton()
#include <netinet/in.h>      // sockaddr_in

#define PORT   3333
#define BUF_SZ 256

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        return 1;
    }

    int sock_fd;
    struct sockaddr_in serv_addr;

    // (a) Create TCP socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // (b) Fill in server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        exit(1);
    }

    // (c) Connect to the server
    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    // (d) Read lines from stdin, send to server, print server’s reply
    char line[BUF_SZ];
    while (1) {
        printf("bank> ");
        if (!fgets(line, BUF_SZ, stdin)) break;  // EOF on stdin

        write(sock_fd, line, strlen(line));

        // Read one line of response
        char resp[BUF_SZ];
        ssize_t n = read(sock_fd, resp, BUF_SZ - 1);
        if (n <= 0) break;   // server closed
        resp[n] = '\0';
        printf("  -> %s", resp);

        // If command was QUIT, exit
        if (strncmp(line, "QUIT", 4) == 0) break;
    }

    close(sock_fd);
    return 0;
}
