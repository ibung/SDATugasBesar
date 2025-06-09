#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umum.h"
#include "citation_stack.h"
#include "avl_paper_loader.h"
#include "pagination.h"

// Forward declarations untuk menghindari circular dependency
typedef struct PaperLoader PaperLoader;
typedef struct AVLNode AVLNode;
typedef struct CitationNode CitationNode;
typedef struct Paper Paper;
typedef struct PaginationSystem PaginationSystem;

// Enum untuk kriteria sorting
typedef enum {
    SORT_BY_YEAR,
    SORT_BY_CITATIONS
} SortCriteria;

// Enum untuk urutan sorting
typedef enum {
    SORT_ASC,   // Ascending - Menaik
    SORT_DESC   // Descending - Menurun
} SortOrder;

// Struct untuk menampung hasil pencarian
typedef struct SearchResult {
    Paper** papers;         // Array dinamis dari pointer Paper
    int count;              // Jumlah paper dalam hasil
    char searchTerm[150];   // Term yang dicari
    char sortCriteria[20];  // Kriteria sorting ("year" atau "citations")
    char sortOrder[10];     // Urutan sorting ("asc" atau "desc")
} SearchResult;

// ========== FUNCTION DECLARATIONS ==========

// Fungsi utama untuk searching dan sorting
SearchResult* performSearchAndSort(PaperLoader* loader, const char* fieldOfStudy, 
                                 SortCriteria criteria, SortOrder order);

// Fungsi untuk traversal dan sorting seluruh AVL tree
SearchResult* traverseAndSortAVL(PaperLoader* loader, SortCriteria criteria, SortOrder order);

// Fungsi untuk mengirim hasil ke sistem paginasi
PaginationSystem* sendToPagination(SearchResult* result);

// Fungsi untuk menampilkan hasil pencarian
void displaySearchResult(const SearchResult* result);

// Fungsi untuk membebaskan memori SearchResult
void freeSearchResult(SearchResult* result);

// Fungsi demonstrasi untuk testing
void demonstrateSearchAndSort(PaperLoader* loader);

// ========== INTERNAL HELPER FUNCTIONS ==========
// (Biasanya static dalam file .c, tapi dideklarasikan di sini untuk fleksibilitas)

// Fungsi untuk membuat SearchResult baru
SearchResult* createSearchResult();

// Fungsi sorting untuk Singly Linked List menggunakan Merge Sort
void sortPaperListSLL(CitationNode** headRef, SortCriteria criteria, SortOrder order);

// Fungsi helper untuk merge sort
CitationNode* sortedMerge(CitationNode* a, CitationNode* b, SortCriteria criteria, SortOrder order);
void splitList(CitationNode* source, CitationNode** frontRef, CitationNode** backRef);

// ========== UTILITY FUNCTIONS ==========

// Fungsi untuk konversi enum ke string
const char* sortCriteriaToString(SortCriteria criteria);
const char* sortOrderToString(SortOrder order);

// Fungsi untuk validasi input
bool isValidSortCriteria(SortCriteria criteria);
bool isValidSortOrder(SortOrder order);

// Fungsi untuk menghitung total paper dalam hasil
int countPapersInResult(const SearchResult* result);

// Fungsi untuk mencari paper berdasarkan index dalam hasil
Paper* getPaperFromResult(const SearchResult* result, int index);

#endif // SEARCH_SORT_H