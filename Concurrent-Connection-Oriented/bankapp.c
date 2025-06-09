#include "bankapp.h"
#include <time.h>

// Define globals
Account *head = NULL;
int account_number_seed = 1001;

// Helper: find an account by number+PIN
Account *find_account(int acct_no, int pin) {
    Account *cur = head;
    while (cur) {
        if (cur->account_number == acct_no && cur->pin == pin) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

// Helper: record a transaction (keeps only last MAX_TRANS)
void record_transaction(Account *acc, const char *type, int amount) {
    // Shift older transactions if at max
    if (acc->trans_count == MAX_TRANS) {
        for (int i = 1; i < MAX_TRANS; i++) {
            acc->transactions[i - 1] = acc->transactions[i];
        }
        acc->trans_count--;
    }
    // Append new
    strcpy(acc->transactions[acc->trans_count].type, type);
    acc->transactions[acc->trans_count].amount = amount;
    acc->trans_count++;
}

// Display the console menu
void display_menu() {
    printf("\n====== Welcome to Simple Bank App ======\n");
    printf("1. Open Account\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Balance\n");
    printf("5. Mini-Statement\n");
    printf("6. Close Account\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
}

// Interactive: open a new account
void open_account() {
    char name[50], nid[20], type[10];
    printf("Enter your name: ");
    scanf("%49s", name);
    printf("Enter your National ID: ");
    scanf("%19s", nid);
    printf("Enter account type (SAVINGS/CURRENT): ");
    scanf("%9s", type);

    int new_acc_no = account_number_seed++;
    int new_pin = rand() % 9000 + 1000;

    Account *acc = (Account*)malloc(sizeof(Account));
    if (!acc) {
        printf("Allocation error!\n");
        return;
    }
    acc->account_number = new_acc_no;
    acc->pin = new_pin;
    strcpy(acc->name, name);
    strcpy(acc->nid, nid);
    strcpy(acc->account_type, type);
    acc->balance = MIN_BALANCE;
    acc->trans_count = 0;
    acc->next = NULL;

    if (head == NULL) {
        head = acc;
    } else {
        Account *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = acc;
    }

    printf("Account created successfully!\n");
    printf("Account Number: %d\nPIN: %d\n", new_acc_no, new_pin);
}

// Interactive: deposit funds
void deposit() {
    int acct_no, pin, amt;
    printf("Enter account number: ");
    scanf("%d", &acct_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    Account *acc = find_account(acct_no, pin);
    if (!acc) {
        printf("Invalid account or PIN!\n");
        return;
    }
    printf("Enter amount to deposit (min %d): ", MIN_WITHDRAW);
    scanf("%d", &amt);
    if (amt < MIN_WITHDRAW) {
        printf("Amount must be at least %d!\n", MIN_WITHDRAW);
        return;
    }
    acc->balance += amt;
    record_transaction(acc, "DEPOSIT", amt);
    printf("Deposit successful! New balance: %d\n", acc->balance);
}

// Interactive: withdraw funds
void withdraw() {
    int acct_no, pin, amt;
    printf("Enter account number: ");
    scanf("%d", &acct_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    Account *acc = find_account(acct_no, pin);
    if (!acc) {
        printf("Invalid account or PIN!\n");
        return;
    }
    printf("Enter amount to withdraw (min %d): ", MIN_WITHDRAW);
    scanf("%d", &amt);
    if (amt < MIN_WITHDRAW) {
        printf("Amount must be at least %d!\n", MIN_WITHDRAW);
        return;
    }
    if (acc->balance - amt < MIN_BALANCE) {
        printf("Cannot withdraw. Minimum balance %d must be maintained!\n", MIN_BALANCE);
        return;
    }
    acc->balance -= amt;
    record_transaction(acc, "WITHDRAW", amt);
    printf("Withdrawal successful! New balance: %d\n", acc->balance);
}

// Interactive: check balance
void balance() {
    int acct_no, pin;
    printf("Enter account number: ");
    scanf("%d", &acct_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    Account *acc = find_account(acct_no, pin);
    if (!acc) {
        printf("Invalid account or PIN!\n");
        return;
    }
    printf("Current balance: %d\n", acc->balance);
}

// Interactive: show mini‑statement
void statement() {
    int acct_no, pin;
    printf("Enter account number: ");
    scanf("%d", &acct_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    Account *acc = find_account(acct_no, pin);
    if (!acc) {
        printf("Invalid account or PIN!\n");
        return;
    }
    if (acc->trans_count == 0) {
        printf("No transactions yet.\n");
        return;
    }
    printf("Last %d transactions:\n", acc->trans_count);
    for (int i = 0; i < acc->trans_count; i++) {
        printf("  %s: %d\n", acc->transactions[i].type, acc->transactions[i].amount);
    }
}

// Interactive: close account
void close_account() {
    int acct_no, pin;
    printf("Enter account number: ");
    scanf("%d", &acct_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);
    Account *prev = NULL, *cur = head;
    while (cur) {
        if (cur->account_number == acct_no && cur->pin == pin) {
            if (prev) {
                prev->next = cur->next;
            } else {
                head = cur->next;
            }
            free(cur);
            printf("Account closed successfully.\n");
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    printf("Invalid account or PIN!\n");
}
