#ifndef IBNU_DLLPagination_H
#define IBNU_DLLPagination_H

#include "umum.h"
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