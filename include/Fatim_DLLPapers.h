#ifndef FATIM_DLLPAPERS_H
#define FATIM_DLLPAPERS_H

#include "umum.h"

// Function declarations
Paper* create_paper(const char* title, const char* field, int year, int citations);
void insert_paper_sorted_by_year(Paper** head, Paper* new_paper);
void insert_paper_end(Paper** head, Paper* new_paper);
void sort_by_citations(Paper** head);
void sort_papers_by_year(Paper** head, int newest_first);
void sort_papers_by_citations(Paper** head, int most_popular_first);
void swap_paper_data(Paper* a, Paper* b);
void display_papers(Paper* head);
void display_papers_compact(Paper* head);
void free_papers_list(Paper* head);

#endif