// ==================== MENU STACK HEADER ====================
// File: Abi_MainProgramStack.h
// Author: Varian Abidarma Syuhada (241511091)
// Description: Stack implementation for menu navigation history

#ifndef MENU_STACK_H
#define MENU_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MenuStack structure - using Stack (LIFO) for menu history
typedef struct MenuStack {
    int menu_id;                    // ID of the menu option
    char menu_name[100];           // Name/description of menu
    struct MenuStack* next;        // Pointer to next item in stack
} MenuStack;

// Function prototypes for menu stack operations
void push_menu(MenuStack** top, int id, const char* name);
MenuStack* pop_menu(MenuStack** top);
void display_menu_history(MenuStack* top);
void clear_menu_history(MenuStack** top);
void free_menu_stack(MenuStack* top);
int get_menu_history_count(MenuStack* top);

#endif // MENU_STACK_H