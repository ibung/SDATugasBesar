#ifndef UMUM_H
#define UMUM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Citation {
    char citing_paper_id[50];
    char citing_paper_title[256];
    int citation_count;
    struct Citation* next;
} Citation;

typedef struct Paper {
    char title[256];
    char field_of_study[100];
    int year;
    int citation_count;
    Citation* citations_head;
    struct Paper* next;
    struct Paper* prev;
} Paper;

// New structures for pagination
typedef struct PaginationTab {
    int tab_number;
    Paper* papers[10];  // Maximum 10 papers per tab
    int paper_count;    // Actual number of papers in this tab
    struct PaginationTab* next;
    struct PaginationTab* prev;
} PaginationTab;

typedef struct PaginationSystem {
    PaginationTab* first_tab;
    PaginationTab* last_tab;
    PaginationTab* current_tab;
    int total_tabs;
    int total_papers;
} PaginationSystem;

#endif