#ifndef IBNU_SLL_H
#define IBNU_SLL_H

#include "umum.h"

// Function declarations
Citation* create_citation(const char* paper_id, const char* title);
void add_citation(Citation** head, Citation* new_citation);
int count_citations(Citation* head);
Citation* merge_sort_citations(Citation* head);
void display_citations(Citation* head);
void free_citations(Citation* head);
Citation* get_middle(Citation* head);
Citation* merge_citations(Citation* left, Citation* right);

#endif