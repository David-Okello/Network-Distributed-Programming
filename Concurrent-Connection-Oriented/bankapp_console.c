#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bankapp.h"

int main() {
    int choice;
    srand((unsigned)time(NULL));

    do {
        display_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: open_account();    break;
            case 2: deposit();         break;
            case 3: withdraw();        break;
            case 4: balance();         break;
            case 5: statement();       break;
            case 6: close_account();   break;
            case 7: exit(0);
            default: printf("Invalid choice\n");
        }
    } while (1);

    return 0; // never reached
}
