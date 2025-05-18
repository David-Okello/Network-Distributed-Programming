#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read, write, close
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton, sockaddr_in

#define PORT 3333
#define BUF_SZ 256

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)};
    inet_pton(AF_INET, argv[1], &serv.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv, sizeof serv) < 0)
    {
        perror("connect");
        exit(1);
    }

    char line[BUF_SZ];
    while (1)
    {
        printf("bank> ");
        if (!fgets(line, BUF_SZ, stdin))
            break;

        write(sock, line, strlen(line));

        char resp[BUF_SZ];
        ssize_t n = read(sock, resp, BUF_SZ - 1);
        if (n <= 0)
            break;
        resp[n] = '\0';
        printf(" -> %s", resp);

        if (strncmp(line, "QUIT", 4) == 0)
            break;
    }

    close(sock);
    return 0;
}
