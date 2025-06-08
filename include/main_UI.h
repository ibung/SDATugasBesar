#ifndef ABI_MAINPROGRAMSTACK_H
#define ABI_MAINPROGRAMSTACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MenuStack {
    int menu_id;
    char menu_name[100];
    struct MenuStack* next;
} MenuStack;

// Function declarations
void push_menu(MenuStack** top, int id, const char* name);
MenuStack* pop_menu(MenuStack** top);
void display_menu_history(MenuStack* top);
void free_menu_stack(MenuStack* top);

#endif