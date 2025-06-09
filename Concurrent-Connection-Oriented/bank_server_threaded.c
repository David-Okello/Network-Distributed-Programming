/*
 * bank_server_threaded.c
 * Concurrent, connection-oriented server using POSIX threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "command_processor.h"

#define PORT     3333
#define BACKLOG  10
#define BUF_SZ   256

void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);
    char buf[BUF_SZ];
    ssize_t n;

    while ((n = read(client_fd, buf, BUF_SZ-1)) > 0) {
        buf[n] = '\0';
        process_command(client_fd, buf);
    }
    close(client_fd);
    return NULL;
}

int main() {
    int listen_fd;
    struct sockaddr_in serv_addr;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket"); exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen"); exit(1);
    }
    printf("Threaded Bank Server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));
        if ((*client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addrlen)) < 0) {
            perror("accept");
            free(client_fd);
            continue;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_fd);
        pthread_detach(tid);
    }
    close(listen_fd);
    return 0;
}
