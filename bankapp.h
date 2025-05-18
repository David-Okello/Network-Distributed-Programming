#ifndef BANKAPP_H
#define BANKAPP_H

// Pure-C account logic lives in bankapp.c
// These wrappers translate socket I/O ↔ your functions:

void open_account_network(const char *name,
                          const char *nid,
                          const char *type,
                          int *acct_no,
                          int *pin);

int deposit_network(int acct_no, int pin, int amount);
int withdraw_network(int acct_no, int pin, int amount);
int balance_network(int acct_no, int pin);
char *statement_network(int acct_no, int pin);
int close_account_network(int acct_no, int pin);

#endif // BANKAPP_H
