#ifndef MAIN_UI_H
#define MAIN_UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umum.h"
#include "json_loader.h"

// MenuStack structure - using Stack (LIFO) for menu history
typedef struct MenuStack {
    int menu_id;                    // ID of the menu option
    char menu_name[100];           // Name/description of menu
    struct MenuStack* next;        // Pointer to next item in stack
} MenuStack;

// Function prototypes for menu stack operations
void mainMenu(PaperLoader *loader);
void searchByField(PaperLoader *loader);
void displayAllFields(PaperLoader *loader);
void displayAllPapersWithSorting(PaperLoader *loader);
void undoSearch(PaperLoader *loader);
Paper *convertCitationsToPaperList(CitationNode *citationHead, int *count);
void handleSortingOptions(Paper *paperList, int count);
void handlePagination(Paper *paperList, int count);
void clearScreen();
void pressEnterToContinue();

#endif // MENU_STACK_H