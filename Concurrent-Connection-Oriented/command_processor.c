/*
 * command_processor.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bankapp.h"
#include "command_processor.h"

static void send_line(int fd, const char *s) {
    write(fd, s, strlen(s));
    write(fd, "\n", 1);
}

void process_command(int client_fd, const char *buf) {
    char cmd[16];
    sscanf(buf, "%15s", cmd);

    if (strcmp(cmd, "OPEN") == 0) {
        char name[64], nid[32], type[16];
        int acct_no, pin;
        sscanf(buf + 5, "%63s %31s %15s", name, nid, type);
        open_account_network(name, nid, type, &acct_no, &pin);
        char resp[64];
        snprintf(resp, sizeof(resp), "OK %d %d", acct_no, pin);
        send_line(client_fd, resp);
    } else if (strcmp(cmd, "DEPOSIT") == 0) {
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
    } else if (strcmp(cmd, "WITHDRAW") == 0) {
        int an, p, amt;
        sscanf(buf + 9, "%d %d %d", &an, &p, &amt);
        int new_bal = withdraw_network(an, p, amt);
        if (new_bal >= 0) {
            char resp[64];
            snprintf(resp, sizeof(resp), "OK %d", new_bal);
            send_line(client_fd, resp);
        } else {
            send_line(client_fd, "ERR withdraw failed");
        }
    } else if (strcmp(cmd, "BALANCE") == 0) {
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
    } else if (strcmp(cmd, "STATEMENT") == 0) {
        int an, p;
        sscanf(buf + 10, "%d %d", &an, &p);
        char *stmt = statement_network(an, p);
        send_line(client_fd, stmt);
        free(stmt);
    } else if (strcmp(cmd, "CLOSE") == 0) {
        int an, p;
        sscanf(buf + 6, "%d %d", &an, &p);
        if (close_account_network(an, p) == 0)
            send_line(client_fd, "OK");
        else
            send_line(client_fd, "ERR close failed");
    } else {
        send_line(client_fd, "ERR unknown command");
    }
}
