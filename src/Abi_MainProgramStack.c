#include "../include/Abi_MainProgramStack.h"

void push_menu(MenuStack** top, int id, const char* name) {
    MenuStack* new_menu = (MenuStack*)malloc(sizeof(MenuStack));
    new_menu->menu_id = id;
    strcpy(new_menu->menu_name, name);
    new_menu->next = *top;
    *top = new_menu;
}

// Fungsi untuk pop menu dari stack (menghapus menu teratas dari history)
MenuStack* pop_menu(MenuStack** top) {
    if (*top == NULL) {
        return NULL;
    }
    
    MenuStack* popped = *top;
    *top = (*top)->next;
    return popped;
}

// Fungsi untuk menampilkan history menu yang telah diakses
void display_menu_history(MenuStack* top) {
    if (top == NULL) {
        printf("No menu history available.\n");
        return;
    }
    
    printf("\n========== MENU HISTORY ==========\n");
    MenuStack* current = top;
    int count = 1;
    
    printf("Recent menu activities (most recent first):\n");
    while (current != NULL) {
        printf("%d. Menu ID: %d - %s\n", count, current->menu_id, current->menu_name);
        current = current->next;
        count++;
    }
    printf("=================================\n\n");
}

// Fungsi untuk membebaskan semua memori dalam menu stack
void free_menu_stack(MenuStack* top) {
    MenuStack* current = top;
    MenuStack* next_menu;
    
    while (current != NULL) {
        next_menu = current->next;
        free(current);
        current = next_menu;
    }
}