// ==================== MENU STACK IMPLEMENTATION ====================
// File: Abi_MainProgramStack.c
// Author: Varian Abidarma Syuhada (241511091)
// Description: Stack implementation for menu navigation history

#include "include/Abi_MainProgramStack.h"

// Push new menu to the top of stack
void push_menu(MenuStack** top, int id, const char* name) {
    // Allocate memory for new menu item
    MenuStack* new_menu = (MenuStack*)malloc(sizeof(MenuStack));
    if (new_menu == NULL) {
        printf("❌ Error: Memory allocation failed for menu stack!\n");
        return;
    }
    
    // Initialize new menu item
    new_menu->menu_id = id;
    strncpy(new_menu->menu_name, name, sizeof(new_menu->menu_name) - 1);
    new_menu->menu_name[sizeof(new_menu->menu_name) - 1] = '\0'; // Ensure null termination
    new_menu->next = *top;
    
    // Update top pointer
    *top = new_menu;
    
    // Optional: Show confirmation (comment out if too verbose)
    // printf("✅ Menu '%s' added to navigation history.\n", name);
}

// Pop menu from the top of stack
MenuStack* pop_menu(MenuStack** top) {
    if (*top == NULL) {
        return NULL;
    }
    
    MenuStack* popped = *top;
    *top = (*top)->next;
    return popped;
}

// Display complete menu history
void display_menu_history(MenuStack* top) {
    if (top == NULL) {
        printf("📝 No menu navigation history available.\n");
        printf("   Start using the system to build your history!\n");
        return;
    }
    
    printf("\n");
    for(int i = 0; i < 50; i++) printf("=");
    printf("\n");
    printf("📋 MENU NAVIGATION HISTORY (Recent → Oldest)\n");
    for(int i = 0; i < 50; i++) printf("=");
    printf("\n");
    
    MenuStack* current = top;
    int count = 1;
    
    while (current != NULL) {
        printf("%2d. [ID: %d] %s\n", count, current->menu_id, current->menu_name);
        current = current->next;
        count++;
    }
    
    for(int i = 0; i < 50; i++) printf("=");
    printf("\n");
    printf("Total menu actions recorded: %d\n", count - 1);
}

// Clear all menu history with confirmation
void clear_menu_history(MenuStack** top) {
    if (*top == NULL) {
        printf("📝 Menu history is already empty.\n");
        return;
    }
    
    int count = get_menu_history_count(*top);
    
    printf("\n🗑️  CLEAR MENU HISTORY\n");
    printf("Current history contains %d entries.\n", count);
    printf("Are you sure you want to clear all navigation history? (y/N): ");
    
    char confirm;
    scanf(" %c", &confirm);
    getchar(); // consume newline
    
    if (confirm == 'y' || confirm == 'Y') {
        free_menu_stack(*top);
        *top = NULL;
        printf("✅ Menu history cleared successfully! (%d entries removed)\n", count);
    } else {
        printf("❌ Operation cancelled. History preserved.\n");
    }
}

// Free entire menu stack
void free_menu_stack(MenuStack* top) {
    while (top != NULL) {
        MenuStack* temp = top;
        top = top->next;
        free(temp);
    }
}

// Get count of items in menu history
int get_menu_history_count(MenuStack* top) {
    int count = 0;
    MenuStack* current = top;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}