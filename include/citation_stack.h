#ifndef CITATION_STACK_H
#define CITATION_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umum.h"

// Struktur untuk Citation (SLL)
typedef struct CitationNode {
    Paper* paper;
    struct CitationNode* next;
} CitationNode;

// Struktur untuk History Stack
typedef struct HistoryNode {
    char action[100];
    char details[200];
    struct HistoryNode* next;
} HistoryNode;

// Struktur utama untuk Citation Manager
typedef struct CitationManager {
    CitationNode* citationHead;
    HistoryNode* historyTop;
    int citationCount;
} CitationManager;

// Function declarations
CitationManager* initCitationManager();
void addCitation(CitationManager* manager, Paper* paper);
void removeCitation(CitationManager* manager, int index);
void displayCitations(CitationManager* manager);
void pushHistory(CitationManager* manager, const char* action, const char* details);
void popHistory(CitationManager* manager);
void displayHistory(CitationManager* manager);
Paper* createPaper(const char* title, const char* author, int year, const char* bidang_studi, int citations);
void freeCitationManager(CitationManager* manager);

#endif