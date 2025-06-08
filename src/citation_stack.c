#include "citation_stack.h"

// Inisialisasi Citation Manager
CitationManager* initCitationManager() {
    CitationManager* manager = (CitationManager*)malloc(sizeof(CitationManager));
    manager->citationHead = NULL;
    manager->historyTop = NULL;
    manager->citationCount = 0;
    return manager;
}

// Membuat paper baru
Paper* createPaper(const char* title, const char* author, int year, const char* journal, int citations) {
    Paper* newPaper = (Paper*)malloc(sizeof(Paper));
    strcpy(newPaper->title, title);
    strcpy(newPaper->author, author);
    newPaper->year = year;
    strcpy(newPaper->journal, journal);
    newPaper->citations = citations;
    newPaper->next = NULL;
    return newPaper;
}

// Menambahkan citation ke SLL
void addCitation(CitationManager* manager, Paper* paper) {
    CitationNode* newNode = (CitationNode*)malloc(sizeof(CitationNode));
    newNode->paper = paper;
    newNode->next = NULL;
    
    if (manager->citationHead == NULL) {
        manager->citationHead = newNode;
    } else {
        CitationNode* temp = manager->citationHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    
    manager->citationCount++;
    
    // Push ke history
    char details[200];
    sprintf(details, "Added citation: %s", paper->title);
    pushHistory(manager, "ADD_CITATION", details);
    
    printf("Citation added successfully!\n");
}

// Menampilkan semua citation
void displayCitations(CitationManager* manager) {
    if (manager->citationHead == NULL) {
        printf("No citations found.\n");
        return;
    }
    
    printf("\n=== CITATION LIST ===\n");
    printf("Total Citations: %d\n\n", manager->citationCount);
    
    CitationNode* current = manager->citationHead;
    int index = 1;
    
    while (current != NULL) {
        Paper* paper = current->paper;
        printf("[%d] %s\n", index, paper->title);
        printf("    Author: %s\n", paper->author);
        printf("    Year: %d\n", paper->year);
        printf("    Journal: %s\n", paper->journal);
        printf("    Citations: %d\n", paper->citations);
        printf("    ------------------------\n");
        
        current = current->next;
        index++;
    }
    
    // Push ke history
    pushHistory(manager, "VIEW_CITATIONS", "Displayed all citations");
}

// Push action ke history stack
void pushHistory(CitationManager* manager, const char* action, const char* details) {
    HistoryNode* newNode = (HistoryNode*)malloc(sizeof(HistoryNode));
    strcpy(newNode->action, action);
    strcpy(newNode->details, details);
    newNode->next = manager->historyTop;
    manager->historyTop = newNode;
}

// Pop (undo) dari history stack
void popHistory(CitationManager* manager) {
    if (manager->historyTop == NULL) {
        printf("No history to undo.\n");
        return;
    }
    
    HistoryNode* temp = manager->historyTop;
    printf("Undoing: %s - %s\n", temp->action, temp->details);
    
    manager->historyTop = manager->historyTop->next;
    free(temp);
    
    printf("Last action undone.\n");
}

// Menampilkan history stack
void displayHistory(CitationManager* manager) {
    if (manager->historyTop == NULL) {
        printf("No history available.\n");
        return;
    }
    
    printf("\n=== SEARCH HISTORY ===\n");
    HistoryNode* current = manager->historyTop;
    int index = 1;
    
    while (current != NULL) {
        printf("[%d] %s: %s\n", index, current->action, current->details);
        current = current->next;
        index++;
    }
    printf("======================\n");
}

// Free memory
void freeCitationManager(CitationManager* manager) {
    // Free citations
    CitationNode* citeCurrent = manager->citationHead;
    while (citeCurrent != NULL) {
        CitationNode* temp = citeCurrent;
        citeCurrent = citeCurrent->next;
        free(temp->paper);
        free(temp);
    }
    
    // Free history
    HistoryNode* histCurrent = manager->historyTop;
    while (histCurrent != NULL) {
        HistoryNode* temp = histCurrent;
        histCurrent = histCurrent->next;
        free(temp);
    }
    
    free(manager);
}