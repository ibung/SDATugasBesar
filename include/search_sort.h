#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include <stdbool.h>

// 1. Panggil header lain untuk mendapatkan definisi struct yang benar
#include "avl_paper_loader.h" 
#include "pagination.h"      
#include "citation_stack.h"  // <-- PENTING: Definisi 'Paper' yang benar berasal dari sini

// 2. PASTIKAN TIDAK ADA LAGI 'typedef struct Paper { ... } Paper;' DI DALAM FILE INI
//    Definisi yang duplikat sudah dihapus.

// Enum untuk kriteria sorting
typedef enum {
    SORT_BY_YEAR,
    SORT_BY_CITATIONS
} SortCriteria;

// Enum untuk urutan sorting
typedef enum {
    SORT_ASC, // Menaik
    SORT_DESC // Menurun
} SortOrder;

// Struct untuk menampung hasil pencarian
typedef struct SearchResult {
    Paper** papers;         // Array dinamis, menggunakan 'Paper' dari citation_stack.h
    int count;              // Jumlah paper dalam hasil
    char searchTerm[150];
    char sortCriteria[20];
    char sortOrder[10];
} SearchResult;


// Deklarasi fungsi-fungsi yang ada di search_sort.c
SearchResult* performSearchAndSort(PaperLoader* loader, const char* fieldOfStudy, SortCriteria criteria, SortOrder order);
SearchResult* traverseAndSortAVL(PaperLoader* loader, SortCriteria criteria, SortOrder order);
PaginationSystem* sendToPagination(SearchResult* result);
void displaySearchResult(const SearchResult* result);
void freeSearchResult(SearchResult* result);
void demonstrateSearchAndSort(PaperLoader* loader);

#endif // SEARCH_SORT_H
