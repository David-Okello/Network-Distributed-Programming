#ifndef BANKAPP_H
#define BANKAPP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_BALANCE   1000
#define MIN_WITHDRAW   500
#define MAX_TRANS      5

typedef struct Transaction {
    char type[10];   // "DEPOSIT" or "WITHDRAW"
    int  amount;
} Transaction;

typedef struct Account {
    int account_number;
    int pin;
    char name[50];
    char nid[20];
    char account_type[10];
    int balance;
    Transaction transactions[MAX_TRANS];
    int trans_count;
    struct Account *next;
} Account;

// Global head pointer and seed for account numbers
extern Account *head;
extern int account_number_seed;

// Core, “pure‑C” functions (interactive console version)
void open_account();
void close_account();
void deposit();
void withdraw();
void balance();
void statement();
Account *find_account(int acct_no, int pin);
void record_transaction(Account *acc, const char *type, int amount);
void display_menu();

// Network‑wrapper function prototypes (used by the TCP server)
void open_account_network(const char *name,
                          const char *nid,
                          const char *type,
                          int *acct_no,
                          int *pin);

int  deposit_network(int acct_no, int pin, int amount);
int  withdraw_network(int acct_no, int pin, int amount);
int  balance_network(int acct_no, int pin);
char* statement_network(int acct_no, int pin);
int  close_account_network(int acct_no, int pin);

#endif // BANKAPP_H
