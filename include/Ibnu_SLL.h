#ifndef Ibnu_SLL_H
#define Ibnu_SLL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Citation {
    char citing_paper_id[50];
    char citing_paper_title[256];
    struct Citation* next;
} Citation;

Citation* create_citation(const char* paper_id, const char* title);
void add_citation(Citation** head, Citation* new_citation);
int count_citations(Citation* head);
void display_citations(Citation* head);
void free_citations(Citation* head);
Citation* merge_sort_citations(Citation* head);
Citation* merge_citations(Citation* left, Citation* right);
Citation* get_middle(Citation* head);

#endif