#ifndef IBNU_SLL_H
#define IBNU_SLL_H

#include "umum.h"

// Function declarations
Citation* create_citation(const char* paper_id, const char* title, int citation_count);
void add_citation(Citation** head, Citation* new_citation);
int count_citations(Citation* head);
Citation* merge_sort_citations(Citation* head);
void display_citations(Citation* head);
void free_citations(Citation* head);
Citation* get_middle(Citation* head);
Citation* merge_citations(Citation* left, Citation* right);

// New pagination functions
PaginationSystem* create_pagination_system();
PaginationTab* create_pagination_tab(int tab_number);
void add_paper_to_pagination(PaginationSystem* pagination, Paper* paper);
void display_pagination_interface(PaginationSystem* pagination);
void navigate_pagination(PaginationSystem* pagination);
void display_current_tab(PaginationSystem* pagination);
void free_pagination_system(PaginationSystem* pagination);
PaginationSystem* convert_papers_to_pagination(Paper** papers_array, int total_papers);

#endif