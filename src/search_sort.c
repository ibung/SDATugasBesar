<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
=======
#include "../include/search_sort.h"
>>>>>>> main

// Panggil semua header yang relevan secara eksplisit
#include "../include/search_sort.h"
#include "../include/avl_paper_loader.h"
#include "../include/pagination.h"
#include "../include/citation_stack.h"
#include "../include/umum.h"

//======================================================================
// BAGIAN 1: IMPLEMENTASI MERGE SORT UNTUK SINGLY-LINKED LIST (SLL)
// Ini adalah implementasi untuk tugas "sortPaperListSLL"
//======================================================================

// Deklarasi forward untuk fungsi-fungsi rekursif internal
static CitationNode* sortedMerge(CitationNode* a, CitationNode* b, SortCriteria criteria, SortOrder order);
static void splitList(CitationNode* source, CitationNode** frontRef, CitationNode** backRef);

/**
 * @brief Fungsi utama untuk mengurutkan SLL dari CitationNode menggunakan Merge Sort.
 * @param headRef Pointer ke pointer head dari SLL.
 * @param criteria Kriteria untuk sorting (SORT_BY_YEAR atau SORT_BY_CITATIONS).
 * @param order Urutan sorting (SORT_ASC atau SORT_DESC).
 */
static void sortPaperListSLL(CitationNode** headRef, SortCriteria criteria, SortOrder order) {
    CitationNode* head = *headRef;
    CitationNode* a;
    CitationNode* b;

    // Kasus dasar: jika list kosong atau hanya punya 1 elemen, list sudah terurut.
    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    // 1. Pisahkan list menjadi dua bagian dari tengah.
    splitList(head, &a, &b);

    // 2. Urutkan kedua sub-list secara rekursif.
    sortPaperListSLL(&a, criteria, order);
    sortPaperListSLL(&b, criteria, order);

    // 3. Gabungkan kedua sub-list yang sudah terurut menjadi satu list utuh.
    *headRef = sortedMerge(a, b, criteria, order);
}

/**
 * @brief Menggabungkan dua SLL yang sudah terurut menjadi satu.
 * @return CitationNode* Head dari list hasil penggabungan yang sudah terurut.
 */
static CitationNode* sortedMerge(CitationNode* a, CitationNode* b, SortCriteria criteria, SortOrder order) {
    CitationNode* result = NULL;

    // Kasus dasar rekursi
    if (a == NULL) return b;
    if (b == NULL) return a;

    // Tentukan kondisi perbandingan berdasarkan kriteria dan urutan
    bool shouldPickA;
    if (criteria == SORT_BY_YEAR) {
        // Jika urutan ASCENDING, pilih tahun yang lebih kecil.
        if (order == SORT_ASC) {
            shouldPickA = (a->paper->year <= b->paper->year);
        } 
        // Jika urutan DESCENDING, pilih tahun yang lebih besar.
        else {
            shouldPickA = (a->paper->year >= b->paper->year);
        }
    } else { // SORT_BY_CITATIONS
        // Jika urutan ASCENDING, pilih sitasi yang lebih sedikit.
        if (order == SORT_ASC) {
            shouldPickA = (a->paper->citation_count <= b->paper->citation_count);
        }
        // Jika urutan DESCENDING, pilih sitasi yang lebih banyak.
        else {
            shouldPickA = (a->paper->citation_count >= b->paper->citation_count);
        }
    }

    // Pilih node 'a' atau 'b' berdasarkan hasil perbandingan, lalu lanjutkan rekursi
    if (shouldPickA) {
        result = a;
        result->next = sortedMerge(a->next, b, criteria, order);
    } else {
        result = b;
        result->next = sortedMerge(a, b->next, criteria, order);
    }
    return result;
}

/**
 * @brief Memisahkan SLL menjadi dua sub-list. Menggunakan metode Fast/Slow Pointer.
 * @param source Head dari list sumber.
 * @param frontRef Pointer untuk menyimpan head dari paruh pertama.
 * @param backRef Pointer untuk menyimpan head dari paruh kedua.
 */
static void splitList(CitationNode* source, CitationNode** frontRef, CitationNode** backRef) {
    CitationNode* fast;
    CitationNode* slow;
    slow = source;
    fast = source->next;

    // Gerakkan 'fast' dua langkah, dan 'slow' satu langkah pada setiap iterasi.
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' sekarang berada di node sebelum titik tengah list.
    *frontRef = source;      // Paruh pertama adalah dari awal sampai 'slow'.
    *backRef = slow->next;   // Paruh kedua adalah sisa list setelah 'slow'.
    slow->next = NULL;       // Putuskan koneksi untuk memisahkan list.
}


//======================================================================
// BAGIAN 2: IMPLEMENTASI FUNGSI UTAMA DAN FUNGSI BANTU
//======================================================================

/**
 * @brief Fungsi internal untuk membuat dan menginisialisasi SearchResult.
 */
static SearchResult* createSearchResult() {
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    if (!result) {
        perror("Gagal alokasi memori untuk SearchResult");
        return NULL;
    }
    result->papers = NULL;
    result->count = 0;
    strcpy(result->searchTerm, "");
    strcpy(result->sortCriteria, "none");
    strcpy(result->sortOrder, "none");
    return result;
}

/**
 * @brief Fungsi utama yang menjalankan alur: Cari di AVL -> Urutkan SLL -> Kirim Hasil.
 */
SearchResult* performSearchAndSort(PaperLoader* loader, const char* fieldOfStudy, SortCriteria criteria, SortOrder order) {
    printf("\n[Mulai] Mencari bidang studi: '%s'...\n", fieldOfStudy);

    // Langkah 1: Searching bidang studi di AVL Tree
    AVLNode* fieldNode = findFieldInAVL(loader->avlRoot, fieldOfStudy);

    if (fieldNode == NULL) {
        printf(" -> [Gagal] Bidang studi '%s' tidak ditemukan.\n", fieldOfStudy);
        return NULL;
    }
    printf(" -> [OK] Bidang studi ditemukan. Terdapat %d paper.\n", fieldNode->paperCount);
    
    // Langkah 2: Sorting hasil (SLL) langsung dari node AVL
    printf(" -> [Proses] Mengurutkan SLL dari node menggunakan Merge Sort...\n");
    sortPaperListSLL(&(fieldNode->paperList), criteria, order);
    printf(" -> [OK] SLL berhasil diurutkan.\n");

    // Langkah 3: Siapkan hasil untuk dikirim ke modul lain (misal: paginasi)
    SearchResult* result = createSearchResult();
    if (!result) return NULL;

    strncpy(result->searchTerm, fieldOfStudy, 149);
    strcpy(result->sortCriteria, (criteria == SORT_BY_YEAR) ? "year" : "citations");
    strcpy(result->sortOrder, (order == SORT_ASC) ? "asc" : "desc");

    // Salin hasil dari SLL yang sudah terurut ke dalam array di SearchResult
    result->papers = (Paper**)malloc(fieldNode->paperCount * sizeof(Paper*));
    if (!result->papers) {
        perror("Gagal alokasi memori untuk array papers");
        free(result);
        return NULL;
    }
    result->count = fieldNode->paperCount;
    
    CitationNode* current = fieldNode->paperList;
    int i = 0;
    while (current != NULL) {
        result->papers[i++] = current->paper;
        current = current->next;
    }

    printf("[Selesai] Proses pencarian dan pengurutan untuk '%s' berhasil.\n", fieldOfStudy);
    return result;
}

/**
 * @brief Implementasi dari 'traverseAndSortAVL'.
 */
SearchResult* traverseAndSortAVL(PaperLoader* loader, SortCriteria criteria, SortOrder order) {
    printf(" -> [Info] Fungsi 'traverseAndSortAVL' dirancang untuk pencarian global (tidak diimplementasikan dalam fokus tugas ini).\n");
    return NULL;
}

/**
 * @brief Mengirimkan hasil pencarian ke modul paginasi (DLL).
 */
PaginationSystem* sendToPagination(SearchResult* result) {
    if (result == NULL || result->count == 0) {
        printf(" -> [Info] Tidak ada hasil untuk dikirim ke paginasi.\n");
        return NULL;
    }

    printf(" -> [Proses] Mengirim %d hasil ke sistem paginasi (DLL)...\n", result->count);
    
    PaginationSystem* pagination = create_pagination_system();
    if(!pagination) {
        perror("Gagal membuat sistem paginasi");
        return NULL;
    }

    for (int i = 0; i < result->count; i++) {
        add_paper_to_pagination(pagination, result->papers[i]);
    }
    
    printf(" -> [OK] Paginasi berhasil dibuat dengan %d tab.\n", pagination->total_tabs);
    return pagination;
}

/**
 * @brief Menampilkan ringkasan hasil pencarian.
 */
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
    printf("------------------------------------------------------\n");

    int limit = (result->count > 5) ? 5 : result->count;
    printf("Menampilkan %d dari %d hasil teratas:\n\n", limit, result->count);

    for (int i = 0; i < limit; i++) {
        Paper* p = result->papers[i];
        printf("[%d] %s\n", i + 1, p->title);
        printf("    Author    : %s\n", p->authors);
        printf("    Tahun     : %d\n", p->year);
        printf("    Sitasi    : %d\n", p->citations_head);
        printf("    Bidang    : %s\n\n", p->field_of_study);
    }
    if (result->count > limit) {
        printf("... dan %d hasil lainnya (siap untuk paginasi).\n", result->count - limit);
    }
    printf("======================================================\n");
}

/**
 * @brief Membebaskan memori yang dialokasikan untuk SearchResult.
 */
void freeSearchResult(SearchResult* result) {
    if (result != NULL) {
        free(result->papers);
        free(result);
        printf(" -> [OK] Memori SearchResult telah dibebaskan.\n");
    }
}