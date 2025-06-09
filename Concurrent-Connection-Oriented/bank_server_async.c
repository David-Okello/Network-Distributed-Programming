/*
 * bank_server_async.c
 * Concurrent, connection-oriented server using select() for asynchronous I/O
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "command_processor.h"

#define PORT     3333
#define BACKLOG  10
#define BUF_SZ   256
#define MAX_CLIENTS  FD_SETSIZE

int main() {
    int listen_fd, max_fd, new_fd;
    int client_fds[MAX_CLIENTS];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t addrlen;
    fd_set master_set, read_fds;
    char buf[BUF_SZ];
    ssize_t n;

    // Initialize listener
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); exit(1); }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listen_fd, BACKLOG);

    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);
    max_fd = listen_fd;
    for (int i = 0; i < MAX_CLIENTS; i++) client_fds[i] = -1;

    printf("Async Bank Server listening on port %d...\n", PORT);

    while (1) {
        read_fds = master_set;
        if (select(max_fd+1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select"); exit(1);
        }
        // check for new connections
        if (FD_ISSET(listen_fd, &read_fds)) {
            addrlen = sizeof(cli_addr);
            new_fd = accept(listen_fd, (struct sockaddr*)&cli_addr, &addrlen);
            if (new_fd >= 0) {
                FD_SET(new_fd, &master_set);
                if (new_fd > max_fd) max_fd = new_fd;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] < 0) { client_fds[i] = new_fd; break; }
                }
            }
        }
        // handle data from clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd < 0) continue;
            if (FD_ISSET(fd, &read_fds)) {
                n = read(fd, buf, BUF_SZ-1);
                if (n <= 0) {
                    close(fd);
                    FD_CLR(fd, &master_set);
                    client_fds[i] = -1;
                } else {
                    buf[n] = '\0';
                    process_command(fd, buf);
                }
            }
        }
    }
    close(listen_fd);
    return 0;
}
