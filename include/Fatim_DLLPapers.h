#ifndef FATIM_DLLPAPERS_H
#define FATIM_DLLPAPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Paper {
    char title[256];
    char field_of_study[100];
    int year;
    int citation_count;
    struct Citation* citations_head;
    struct Paper* next;
    struct Paper* prev;
} Paper;

Paper* create_paper(const char* title, const char* field, int year, int citations);
void insert_paper_sorted_by_year(Paper** head, Paper* new_paper);
void insert_paper_end(Paper** head, Paper* new_paper);
void sort_by_citations(Paper** head);
void display_papers(Paper* head);
void free_papers_list(Paper* head);

#endif