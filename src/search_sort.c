#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/search_sort.h"
#include "../include/avl_paper_loader.h"
#include "../include/pagination.h"
#include "../include/citation_stack.h"

// ========== FUNGSI INTERNAL (STATIC) ==========

static SearchResult* createSearchResult() {
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    if (!result) return NULL;

    result->papers = NULL;
    result->count = 0;
    strcpy(result->searchTerm, "");
    strcpy(result->sortCriteria, "none");
    strcpy(result->sortOrder, "none");
    
    return result;
}

/**
 * @brief Menambahkan satu paper ke dalam array dinamis di SearchResult.
 */
static void addPaperToResult(SearchResult* result, Paper* paper) {
    if (!result || !paper) return;

    // Menambah ukuran array 'papers' untuk menampung pointer baru
    result->count++;
    result->papers = (Paper**)realloc(result->papers, result->count * sizeof(Paper*));
    if (!result->papers) {
        perror("Gagal realloc memori untuk hasil pencarian");
        exit(EXIT_FAILURE);
    }
    result->papers[result->count - 1] = paper;
}

// ========== FUNGSI PERBANDINGAN UNTUK QSORT ==========

// Fungsi pembanding untuk mengurutkan berdasarkan tahun.
static int compareByYear(const void* a, const void* b) {
    Paper* paperA = *(Paper**)a;
    Paper* paperB = *(Paper**)b;
    return (paperA->year - paperB->year);
}

// Fungsi pembanding untuk mengurutkan berdasarkan jumlah sitasi.
static int compareByCitations(const void* a, const void* b) {
    Paper* paperA = *(Paper**)a;
    Paper* paperB = *(Paper**)b;
    return (paperA->citations - paperB->citations);
}

// ========== IMPLEMENTASI FUNGSI UTAMA ==========

SearchResult* performSearchAndSort(PaperLoader* loader, const char* fieldOfStudy, SortCriteria criteria, SortOrder order) {
    printf("\n🔍 Mencari bidang studi: '%s'...\n", fieldOfStudy);

    AVLNode* fieldNode = findFieldInAVL(loader->avlRoot, fieldOfStudy);

    if (fieldNode == NULL) {
        printf("❌ Bidang studi '%s' tidak ditemukan dalam data.\n", fieldOfStudy);
        return NULL;
    }

    printf("✅ Bidang studi ditemukan. Terdapat %d paper.\n", fieldNode->paperCount);

    SearchResult* result = createSearchResult();
    strncpy(result->searchTerm, fieldOfStudy, sizeof(result->searchTerm) - 1);
    
    // Menyalin pointer dari SLL (paperList) ke array dinamis di SearchResult
    CitationNode* current = fieldNode->paperList;
    while (current != NULL) {
        addPaperToResult(result, current->paper);
        current = current->next;
    }

    printf("📊 Mengurutkan %d hasil...\n", result->count);
    
    // Memilih fungsi pembanding berdasarkan kriteria
    int (*comparator)(const void*, const void*) = NULL;
    if (criteria == SORT_BY_YEAR) {
        comparator = compareByYear;
        strcpy(result->sortCriteria, "year");
    } else { // SORT_BY_CITATIONS
        comparator = compareByCitations;
        strcpy(result->sortCriteria, "citations");
    }

    // Menggunakan qsort untuk pengurutan yang efisien
    qsort(result->papers, result->count, sizeof(Paper*), comparator);

    // qsort selalu ascending, jadi kita balik arraynya jika urutan descending diperlukan
    if (order == SORT_DESC) {
        strcpy(result->sortOrder, "desc");
        int i = 0;
        int j = result->count - 1;
        while (i < j) {
            Paper* temp = result->papers[i];
            result->papers[i] = result->papers[j];
            result->papers[j] = temp;
            i++;
            j--;
        }
    } else {
        strcpy(result->sortOrder, "asc");
    }
    
    printf("✅ Pengurutan selesai. Kriteria: '%s', Urutan: '%s'.\n", result->sortCriteria, result->sortOrder);
    return result;
}

PaginationSystem* sendToPagination(SearchResult* result) {
    if (result == NULL || result->count == 0) {
        printf("📤 Tidak ada hasil untuk dikirim ke sistem paginasi.\n");
        return NULL;
    }

    printf("📤 Mengirim %d hasil ke modul paginasi...\n", result->count);
    
    PaginationSystem* pagination = create_pagination_system();
    for (int i = 0; i < result->count; i++) {
        add_paper_to_pagination(pagination, result->papers[i]);
    }
    
    printf("✅ Sistem paginasi berhasil dibuat dengan total %d tab.\n", pagination->total_tabs);
    return pagination;
}


// ========== IMPLEMENTASI FUNGSI BANTU DAN DEMO ==========

void displaySearchResult(const SearchResult* result) {
    if (result == NULL || result->count == 0) {
        printf("\n--- Tidak ada hasil untuk ditampilkan ---\n");
        return;
    }

    printf("\n======================================================\n");
    printf("            HASIL PENCARIAN & SORTING\n");
    printf("======================================================\n");
    printf("Kueri         : %s\n", result->searchTerm);
    printf("Jumlah Hasil  : %d\n", result->count);
    printf("Diurutkan oleh: %s (%s)\n", result->sortCriteria, result->sortOrder);
    printf("------------------------------------------------------\n\n");

    int display_limit = (result->count > 5) ? 5 : result->count; // Tampilkan 5 pertama saja
    printf("Menampilkan %d dari %d hasil teratas:\n\n", display_limit, result->count);

    for (int i = 0; i < display_limit; i++) {
        Paper* p = result->papers[i];
        printf("[%d] %s\n", i + 1, p->title);
        printf("    Author    : %s\n", p->author);
        printf("    Tahun     : %d\n", p->year);
        printf("    Sitasi    : %d\n", p->citations);
        printf("    Bidang    : %s\n\n", p->bidang_studi);
    }

    if (result->count > display_limit) {
        printf("... dan %d hasil lainnya akan ditampilkan melalui paginasi.\n", result->count - display_limit);
    }
    printf("======================================================\n");
}

void freeSearchResult(SearchResult* result) {
    if (result != NULL) {
        free(result->papers); // Membebaskan array dari pointer
        free(result);         // Membebaskan struct itu sendiri
        printf("🧹 Memori untuk SearchResult telah dibebaskan.\n");
    }
}

void demonstrateSearchAndSort(PaperLoader* loader) {
    printf("\n\n=============== DEMO SEARCHING & SORTING ENGINE ===============\n");

    // Skenario 1: Mencari 'Computer Science', urutkan berdasarkan sitasi terbanyak
    printf("\n--- SKENARIO 1: Mencari 'Computer Science', urutkan berdasarkan sitasi (DESC) ---\n");
    SearchResult* result1 = performSearchAndSort(loader, "Computer Science", SORT_BY_CITATIONS, SORT_DESC);
    
    if (result1) {
        displaySearchResult(result1);
        
        PaginationSystem* pagination1 = sendToPagination(result1);
        if (pagination1) {
            printf("\nTekan Enter untuk menampilkan paginasi interaktif...");
            getchar(); // Menunggu input pengguna
            navigate_pagination(pagination1); // Fungsi dari modul paginasi
            free_pagination_system(pagination1); // Membersihkan memori paginasi
        }
        freeSearchResult(result1); // Membersihkan memori hasil pencarian
    }

    // Skenario 2: Mencari 'Mathematics', urutkan berdasarkan tahun terlama
    printf("\n\n--- SKENARIO 2: Mencari 'Mathematics', urutkan berdasarkan tahun (ASC) ---\n");
    SearchResult* result2 = performSearchAndSort(loader, "Mathematics", SORT_BY_YEAR, SORT_ASC);
    if (result2) {
        displaySearchResult(result2);
        freeSearchResult(result2);
    }

    printf("\n=============== DEMO SELESAI ===============\n");
}