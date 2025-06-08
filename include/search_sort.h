#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include "avl_paper_loader.h" 
#include "pagination.h"      
#include "citation_stack.h"  

// Enum untuk kriteria sorting, membuatnya lebih mudah dibaca dan dikelola.
typedef enum {
    SORT_BY_YEAR,
    SORT_BY_CITATIONS
} SortCriteria;

// Enum untuk urutan sorting.
typedef enum {
    SORT_ASC, // Menaik
    SORT_DESC // Menurun
} SortOrder;


typedef struct SearchResult {
    Paper** papers;               // Array dinamis dari pointer ke Paper
    int count;                   // Jumlah total hasil pencarian
    char searchTerm[150];        // Term pencarian yang digunakan
    char sortCriteria[20];       // Kriteria pengurutan (year atau citations)
    char sortOrder[10];          // Urutan pengurutan (asc atau desc)
} SearchResult;

typedef struct Paper {
    char title[200];
    char author[100];         // <-- Nama field: author
    int year;
    char bidang_studi[150];   // <-- Nama field: bidang_studi
    int citations;           // <-- Nama field: citations
    struct Paper* next;
} Paper;


// ========== DEKLARASI FUNGSI UTAMA ==========


SearchResult* performSearchAndSort(PaperLoader* loader, const char* fieldOfStudy, SortCriteria criteria, SortOrder order);

PaginationSystem* sendToPagination(SearchResult* result);


// ========== DEKLARASI FUNGSI BANTU DAN DEMO ========//
void displaySearchResult(const SearchResult* result);

void freeSearchResult(SearchResult* result);

void demonstrateSearchAndSort(PaperLoader* loader);

#endif // SEARCH_SORT_H