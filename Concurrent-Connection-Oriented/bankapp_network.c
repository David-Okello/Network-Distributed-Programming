#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bankapp.h"

// Bring in the global list and seed from bankapp.c
extern Account *head;
extern int account_number_seed;

//
// 1) Create an account (prepending to the list for simplicity):
//
void open_account_network(const char *name,
                          const char *nid,
                          const char *type,
                          int *acct_no,
                          int *pin)
{
    int new_acc_no = account_number_seed++;
    int new_pin    = rand() % 9000 + 1000;  // 4‑digit PIN

    Account *acc = (Account*)malloc(sizeof(Account));
    if (!acc) {
        *acct_no = -1;
        *pin     = -1;
        return;
    }

    acc->account_number = new_acc_no;
    acc->pin            = new_pin;
    strncpy(acc->name, name, sizeof(acc->name)-1);
    acc->name[sizeof(acc->name)-1] = '\0';
    strncpy(acc->nid, nid, sizeof(acc->nid)-1);
    acc->nid[sizeof(acc->nid)-1] = '\0';
    strncpy(acc->account_type, type, sizeof(acc->account_type)-1);
    acc->account_type[sizeof(acc->account_type)-1] = '\0';
    acc->balance     = MIN_BALANCE;
    acc->trans_count = 0;

    // Prepend to list
    acc->next = head;
    head = acc;

    // Debug print
    printf("[DEBUG] open_account_network: now head = %d, PIN = %d\n",
           acc->account_number, acc->pin);
    fflush(stdout);

    *acct_no = new_acc_no;
    *pin     = new_pin;
}

//
// 2) Deposit: find account + PIN, then add amount:
//
int deposit_network(int acct_no, int pin, int amount)
{
    printf("[DEBUG] Trying deposit_network( acct_no=%d, pin=%d, amount=%d )\n",
           acct_no, pin, amount);
    fflush(stdout);

    if (amount < MIN_WITHDRAW) {
        printf("[DEBUG]  -> amount < MIN_WITHDRAW (%d)\n", MIN_WITHDRAW);
        fflush(stdout);
        return -1;
    }

    Account *acc = find_account(acct_no, pin);
    if (!acc) {
        printf("[DEBUG]  -> find_account returned NULL!\n");
        fflush(stdout);
        return -1;
    }

    acc->balance += amount;
    record_transaction(acc, "DEPOSIT", amount);
    printf("[DEBUG]  -> New balance = %d\n", acc->balance);
    fflush(stdout);

    return acc->balance;
}

//
// 3) Withdraw: find account + PIN, check min & remaining balance:
//
int withdraw_network(int acct_no, int pin, int amount)
{
    Account *acc = find_account(acct_no, pin);
    if (!acc) return -1;  // invalid acct or PIN

    if (amount < MIN_WITHDRAW) {
        return -1;  // withdraw must be at least MIN_WITHDRAW
    }
    if (acc->balance - amount < MIN_BALANCE) {
        return -1;  // can’t go below MIN_BALANCE
    }

    acc->balance -= amount;
    record_transaction(acc, "WITHDRAW", amount);
    return acc->balance;
}

//
// 4) Balance: just return current balance, or -1 on invalid:
//
int balance_network(int acct_no, int pin)
{
    Account *acc = find_account(acct_no, pin);
    if (!acc) return -1;
    return acc->balance;
}

//
// 5) Statement: build a string listing the last up to MAX_TRANS transactions:
//
char *statement_network(int acct_no, int pin)
{
    Account *acc = find_account(acct_no, pin);
    if (!acc) return NULL;

    int needed = acc->trans_count * 32 + 1;
    char *buf = (char*)malloc(needed);
    if (!buf) return NULL;
    buf[0] = '\0';

    for (int i = 0; i < acc->trans_count; i++) {
        char line[64];
        snprintf(line, sizeof(line), "%s:%d\n",
                 acc->transactions[i].type,
                 acc->transactions[i].amount);
        strncat(buf, line, needed - strlen(buf) - 1);
    }
    return buf;  // caller must free()
}

//
// 6) Close account: unlink from list + free, return 0 on success or -1 on failure:
//
int close_account_network(int acct_no, int pin)
{
    Account *prev = NULL, *cur = head;
    while (cur) {
        if (cur->account_number == acct_no && cur->pin == pin) {
            if (prev) {
                prev->next = cur->next;
            } else {
                head = cur->next;
            }
            free(cur);
            return 0;
        }
        prev = cur;
        cur  = cur->next;
    }
    return -1;  // not found or bad PIN
}
