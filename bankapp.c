/*
 * Simple Bank App in Pure C
 * Features:
 *  - Open and close accounts
 *  - Withdraw and deposit funds
 *  - Check balance and mini statement (last 5 transactions)
 *  - Maintains a linked list of accounts
 */

#include <stdio.h>  // Standard I/O functions
#include <stdlib.h> // malloc, free, rand, srand, exit
#include <string.h> // strncpy, strcmp
#include <time.h>   // time for srand seed

#define MIN_BALANCE 1000 // Minimum required balance
#define MIN_WITHDRAW 500 // Minimum withdrawal amount
#define MIN_DEPOSIT 500  // Minimum deposit amount
#define MAX_TRANS 5      // Max number of transactions in mini statement

#ifdef _WIN32
#define CLEAR_SCREEN "cls" // Command to clear screen on Windows
#else
#define CLEAR_SCREEN "clear" // Command to clear screen on Unix/Linux
#endif

// Structure for a single transaction record
typedef struct Transaction
{
    char type[10]; // "Credit" or "Debit"
    int amount;    // Amount of the transaction
} Transaction;

// Structure representing a bank account
typedef struct Account
{
    char name[50];                       // Account holder's name
    char national_id[20];                // National ID or similar identifier
    char account_type[10];               // Type: "savings" or "checking"
    int account_number;                  // Unique account number
    int pin;                             // 4-digit PIN for authentication
    int balance;                         // Current account balance
    Transaction transactions[MAX_TRANS]; // Circular buffer for last transactions
    int trans_count;                     // Number of stored transactions
    struct Account *next;                // Pointer to next account in linked list
} Account;

// Global head pointer for the linked list of accounts
Account *head = NULL;
// Seed for generating unique account numbers
int account_number_seed = 1001;

/* Function prototypes */
void clear_screen();
void display_menu();
void open_account();
void close_account();
void withdraw();
void deposit();
void balance();
void statement();
Account *find_account(int account_number, int pin);
void record_transaction(Account *acc, char *type, int amount);

int main()
{
    int choice;
    // Seed random number generator for PIN generation
    srand((unsigned)time(NULL));

    while (1)
    {
        display_menu(); // Show the main menu
        if (scanf("%d", &choice) != 1)
        { // Read user choice
            printf("Invalid input!\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            continue;
        }
        switch (choice)
        {
        case 1:
            open_account();
            break; // Create new account
        case 2:
            close_account();
            break; // Close existing account
        case 3:
            withdraw();
            break; // Withdraw funds
        case 4:
            deposit();
            break; // Deposit funds
        case 5:
            balance();
            break; // Check balance
        case 6:
            statement();
            break; // Show mini statement
        case 7:
            printf("Exiting...\n");
            exit(0);
        default:
            printf("Invalid choice!\n");
        }
    }
    return 0;
}

// Clear terminal screen
void clear_screen()
{
    system(CLEAR_SCREEN);
}

// Display menu options to the user
void display_menu()
{
    clear_screen();
    printf("===== Simple Bank App =====\n");
    printf("1. Open Account\n");
    printf("2. Close Account\n");
    printf("3. Withdraw\n");
    printf("4. Deposit\n");
    printf("5. Check Balance\n");
    printf("6. Mini Statement\n");
    printf("7. Exit\n");
    printf("Choose an option: ");
}

// Create a new account and add to list
void open_account()
{
    clear_screen();
    Account *new_account = malloc(sizeof(Account)); // Allocate memory

    printf("Enter Name: ");
    scanf("%49s", new_account->name); // Read name

    printf("Enter National ID: ");
    scanf("%19s", new_account->national_id); // Read ID

    printf("Enter Account Type (savings/checking): ");
    scanf("%9s", new_account->account_type); // Read type

    new_account->balance = MIN_BALANCE;                  // Initial balance
    new_account->account_number = account_number_seed++; // Unique number
    new_account->pin = rand() % 9000 + 1000;             // Random 4-digit PIN
    new_account->trans_count = 0;                        // No transactions yet

    // Insert new account at the head of the linked list
    new_account->next = head;
    head = new_account;

    // Show account details
    printf("\nAccount created successfully!\n");
    printf("Account Number: %d\n", new_account->account_number);
    printf("PIN: %d\n", new_account->pin);
    printf("Initial Balance: Ksh %d\n", new_account->balance);
    printf("Press Enter to continue...");
    getchar();
    getchar(); // Pause
}

// Find account by account number and PIN
Account *find_account(int account_number, int pin)
{
    Account *curr = head;
    while (curr)
    {
        if (curr->account_number == account_number && curr->pin == pin)
            return curr; // Found matching account
        curr = curr->next;
    }
    return NULL; // Not found
}

// Close an account and free memory
void close_account()
{
    clear_screen();
    int acc_no, pin;
    printf("Enter Account Number: ");
    scanf("%d", &acc_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    Account *prev = NULL, *curr = head;
    while (curr)
    {
        if (curr->account_number == acc_no && curr->pin == pin)
        {
            // Remove from list
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;

            printf("\nAccount %d closed. Final balance Ksh %d returned.\n",
                   curr->account_number, curr->balance);
            free(curr); // Free memory
            printf("Press Enter to continue...");
            getchar();
            getchar();
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    printf("\nAccount not found or invalid PIN.\n");
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Withdraw funds from an account
void withdraw()
{
    clear_screen();
    int acc_no, pin, amount;
    printf("Enter Account Number: ");
    scanf("%d", &acc_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    Account *acc = find_account(acc_no, pin);
    if (!acc)
    {
        printf("\nAccount not found or invalid PIN.\n");
        printf("Press Enter to continue...");
        getchar();
        getchar();
        return;
    }

    printf("Enter amount to withdraw (min Ksh 500): ");
    scanf("%d", &amount);

    if (amount < MIN_WITHDRAW)
    {
        printf("\nInvalid withdraw amount.\n");
    }
    else if (acc->balance - amount < MIN_BALANCE)
    {
        printf("\nInsufficient funds to maintain minimum balance of Ksh %d.\n", MIN_BALANCE);
    }
    else
    {
        acc->balance -= amount;                   // Deduct amount
        record_transaction(acc, "Debit", amount); // Log transaction
        printf("\nWithdrawal successful! New Balance: Ksh %d\n", acc->balance);
    }
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Deposit funds into an account
void deposit()
{
    clear_screen();
    int acc_no, pin, amount;
    printf("Enter Account Number: ");
    scanf("%d", &acc_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    Account *acc = find_account(acc_no, pin);
    if (!acc)
    {
        printf("\nAccount not found or invalid PIN.\n");
        printf("Press Enter to continue...");
        getchar();
        getchar();
        return;
    }

    printf("Enter amount to deposit (min Ksh 500): ");
    scanf("%d", &amount);

    if (amount < MIN_DEPOSIT)
    {
        printf("\nInvalid deposit amount.\n");
    }
    else
    {
        acc->balance += amount;                    // Add amount
        record_transaction(acc, "Credit", amount); // Log transaction
        printf("\nDeposit successful! New Balance: Ksh %d\n", acc->balance);
    }
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Check and display current balance
void balance()
{
    clear_screen();
    int acc_no, pin;
    printf("Enter Account Number: ");
    scanf("%d", &acc_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    Account *acc = find_account(acc_no, pin);
    if (!acc)
    {
        printf("\nAccount not found or invalid PIN.\n");
    }
    else
    {
        printf("\nCurrent Balance for account %d: Ksh %d\n",
               acc->account_number, acc->balance);
    }
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Show the mini statement (last up to 5 transactions)
void statement()
{
    clear_screen();
    int acc_no, pin;
    printf("Enter Account Number: ");
    scanf("%d", &acc_no);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    Account *acc = find_account(acc_no, pin);
    if (!acc)
    {
        printf("\nAccount not found or invalid PIN.\n");
    }
    else if (acc->trans_count == 0)
    {
        printf("\nNo transactions yet.\n");
    }
    else
    {
        printf("\nLast %d Transactions for account %d:\n",
               acc->trans_count, acc->account_number);
        printf("Type      Amount\n");
        printf("----      ------\n");
        for (int i = 0; i < acc->trans_count; i++)
        {
            printf("%-10s Ksh %d\n",
                   acc->transactions[i].type,
                   acc->transactions[i].amount);
        }
    }
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Record a transaction into the account's history buffer
void record_transaction(Account *acc, char *type, int amount)
{
    if (acc->trans_count == MAX_TRANS)
    {
        // Remove oldest transaction by shifting
        for (int i = 1; i < MAX_TRANS; i++)
        {
            acc->transactions[i - 1] = acc->transactions[i];
        }
        acc->trans_count--;
    }
    // Store new transaction at the end
    strncpy(acc->transactions[acc->trans_count].type, type, 9);
    acc->transactions[acc->trans_count].type[9] = '\0';
    acc->transactions[acc->trans_count].amount = amount;
    acc->trans_count++;
}
