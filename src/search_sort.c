#include "../include/search_sort.h"
#include "../include/umum.h"             // Harus pertama untuk definisi dasar
#include "../include/citation_stack.h"   // Untuk Paper dan CitationNode
#include "../include/avl_paper_loader.h" // Untuk PaperLoader dan AVLNode - harus setelah umum.h
#include "../include/pagination.h"       // Untuk PaginationSystem
#include "../include/json_loader.h"      // Untuk JSONPaper


//======================================================================
// BAGIAN 1: IMPLEMENTASI MERGE SORT UNTUK SINGLY-LINKED LIST (SLL)
// Implementasi untuk tugas "sortPaperListSLL"
//======================================================================

// Deklarasi forward untuk fungsi-fungsi rekursif internal
static CitationNode *sortedMergeInternal(CitationNode *a, CitationNode *b, SortCriteria criteria, SortOrder order);
static void splitListInternal(CitationNode *source, CitationNode **frontRef, CitationNode **backRef);

/**
 * @brief Fungsi utama untuk mengurutkan SLL dari CitationNode menggunakan Merge Sort.
 * @param headRef Pointer ke pointer head dari SLL.
 * @param criteria Kriteria untuk sorting (SORT_BY_YEAR atau SORT_BY_CITATIONS).
 * @param order Urutan sorting (SORT_ASC atau SORT_DESC).
 */
void sortPaperListSLL(CitationNode **headRef, SortCriteria criteria, SortOrder order)
{
    CitationNode *head = *headRef;
    CitationNode *a;
    CitationNode *b;

    // Kasus dasar: jika list kosong atau hanya punya 1 elemen, list sudah terurut.
    if ((head == NULL) || (head->next == NULL))
    {
        return;
    }

    // 1. Pisahkan list menjadi dua bagian dari tengah.
    splitListInternal(head, &a, &b);

    // 2. Urutkan kedua sub-list secara rekursif.
    sortPaperListSLL(&a, criteria, order);
    sortPaperListSLL(&b, criteria, order);

    // 3. Gabungkan kedua sub-list yang sudah terurut menjadi satu list utuh.
    *headRef = sortedMergeInternal(a, b, criteria, order);
}

/**
 * @brief Menggabungkan dua SLL yang sudah terurut menjadi satu.
 * @return CitationNode* Head dari list hasil penggabungan yang sudah terurut.
 */
static CitationNode *sortedMergeInternal(CitationNode *a, CitationNode *b, SortCriteria criteria, SortOrder order)
{
    CitationNode *result = NULL;

    // Kasus dasar rekursi
    if (a == NULL)
        return b;
    if (b == NULL)
        return a;

    // Tentukan kondisi perbandingan berdasarkan kriteria dan urutan
    bool shouldPickA;
    if (criteria == SORT_BY_YEAR)
    {
        // Jika urutan ASCENDING, pilih tahun yang lebih kecil.
        if (order == SORT_ASC)
        {
            shouldPickA = (a->paper->year <= b->paper->year);
        }
        // Jika urutan DESCENDING, pilih tahun yang lebih besar.
        else
        {
            shouldPickA = (a->paper->year >= b->paper->year);
        }
    }
    else
    { // SORT_BY_CITATIONS
        // Jika urutan ASCENDING, pilih sitasi yang lebih sedikit.
        if (order == SORT_ASC)
        {
            shouldPickA = (a->paper->citation_count <= b->paper->citation_count);
        }
        // Jika urutan DESCENDING, pilih sitasi yang lebih banyak.
        else
        {
            shouldPickA = (a->paper->citation_count >= b->paper->citation_count);
        }
    }

    // Pilih node 'a' atau 'b' berdasarkan hasil perbandingan, lalu lanjutkan rekursi
    if (shouldPickA)
    {
        result = a;
        result->next = sortedMergeInternal(a->next, b, criteria, order);
    }
    else
    {
        result = b;
        result->next = sortedMergeInternal(a, b->next, criteria, order);
    }
    return result;
}

/**
 * @brief Memisahkan SLL menjadi dua sub-list. Menggunakan metode Fast/Slow Pointer.
 * @param source Head dari list sumber.
 * @param frontRef Pointer untuk menyimpan head dari paruh pertama.
 * @param backRef Pointer untuk menyimpan head dari paruh kedua.
 */
static void splitListInternal(CitationNode *source, CitationNode **frontRef, CitationNode **backRef)
{
    CitationNode *fast;
    CitationNode *slow;
    slow = source;
    fast = source->next;

    // Gerakkan 'fast' dua langkah, dan 'slow' satu langkah pada setiap iterasi.
    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // 'slow' sekarang berada di node sebelum titik tengah list.
    *frontRef = source;    // Paruh pertama adalah dari awal sampai 'slow'.
    *backRef = slow->next; // Paruh kedua adalah sisa list setelah 'slow'.
    slow->next = NULL;     // Putuskan koneksi untuk memisahkan list.
}

/**
 * @brief Implementasi public untuk sortedMerge (wrapper untuk internal function).
 */
CitationNode *sortedMerge(CitationNode *a, CitationNode *b, SortCriteria criteria, SortOrder order)
{
    return sortedMergeInternal(a, b, criteria, order);
}

/**
 * @brief Implementasi public untuk splitList (wrapper untuk internal function).
 */
void splitList(CitationNode *source, CitationNode **frontRef, CitationNode **backRef)
{
    splitListInternal(source, frontRef, backRef);
}

//======================================================================
// BAGIAN 2: IMPLEMENTASI FUNGSI UTAMA DAN FUNGSI BANTU
//======================================================================

/**
 * @brief Fungsi untuk membuat dan menginisialisasi SearchResult.
 */
SearchResult *createSearchResult()
{
    SearchResult *result = (SearchResult *)malloc(sizeof(SearchResult));
    if (!result)
    {
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
SearchResult *performSearchAndSort(PaperLoader *loader, const char *fieldOfStudy, SortCriteria criteria, SortOrder order)
{
    if (!loader || !fieldOfStudy)
    {
        printf("[Error] Parameter tidak valid.\n");
        return NULL;
    }

    printf("\n[Mulai] Mencari bidang studi: '%s'...\n", fieldOfStudy);

    // Langkah 1: Searching bidang studi di AVL Tree
    AVLNode *fieldNode = findFieldInAVL(loader->avlRoot, fieldOfStudy);

    if (fieldNode == NULL)
    {
        printf(" -> [Gagal] Bidang studi '%s' tidak ditemukan.\n", fieldOfStudy);
        return NULL;
    }
    printf(" -> [OK] Bidang studi ditemukan. Terdapat %d paper.\n", fieldNode->paperCount);

    // Langkah 2: Sorting hasil (SLL) langsung dari node AVL
    printf(" -> [Proses] Mengurutkan SLL dari node menggunakan Merge Sort...\n");
    sortPaperListSLL(&(fieldNode->paperList), criteria, order);
    printf(" -> [OK] SLL berhasil diurutkan.\n");

    // Langkah 3: Siapkan hasil untuk dikirim ke modul lain (misal: paginasi)
    SearchResult *result = createSearchResult();
    if (!result)
        return NULL;

    strncpy(result->searchTerm, fieldOfStudy, 149);
    result->searchTerm[149] = '\0'; // Pastikan null-terminated
    strcpy(result->sortCriteria, (criteria == SORT_BY_YEAR) ? "year" : "citations");
    strcpy(result->sortOrder, (order == SORT_ASC) ? "asc" : "desc");

    // Salin hasil dari SLL yang sudah terurut ke dalam array di SearchResult
    result->papers = (Paper **)malloc(fieldNode->paperCount * sizeof(Paper *));
    if (!result->papers)
    {
        perror("Gagal alokasi memori untuk array papers");
        free(result);
        return NULL;
    }
    result->count = fieldNode->paperCount;

    CitationNode *current = fieldNode->paperList;
    int i = 0;
    while (current != NULL && i < fieldNode->paperCount)
    {
        result->papers[i++] = current->paper;
        current = current->next;
    }

    printf("[Selesai] Proses pencarian dan pengurutan untuk '%s' berhasil.\n", fieldOfStudy);
    return result;
}

/**
 * @brief Implementasi dari 'traverseAndSortAVL'.
 */
SearchResult *traverseAndSortAVL(PaperLoader *loader, SortCriteria criteria, SortOrder order)
{
    if (!loader)
    {
        printf("[Error] PaperLoader tidak valid.\n");
        return NULL;
    }

    printf(" -> [Info] Fungsi 'traverseAndSortAVL' untuk pencarian global.\n");
    printf(" -> [Info] Fitur ini dapat diimplementasikan untuk sorting seluruh database.\n");
    printf(" -> [Info] Kriteria: %s, Urutan: %s\n", 
           sortCriteriaToString(criteria), 
           sortOrderToString(order));

    // TODO: Implementasi untuk traversal seluruh AVL tree
    // Saat ini mengembalikan NULL sebagai placeholder
    return NULL;
}

/**
 * @brief Mengirimkan hasil pencarian ke modul paginasi (DLL).
 */
PaginationSystem *sendToPagination(SearchResult *result)
{
    if (result == NULL || result->count == 0)
    {
        printf(" -> [Info] Tidak ada hasil untuk dikirim ke paginasi.\n");
        return NULL;
    }

    printf(" -> [Proses] Mengirim %d hasil ke sistem paginasi (DLL)...\n", result->count);

    PaginationSystem *pagination = create_pagination_system();
    if (!pagination)
    {
        perror("Gagal membuat sistem paginasi");
        return NULL;
    }

    for (int i = 0; i < result->count; i++)
    {
        add_paper_to_pagination(pagination, result->papers[i]);
    }

    printf(" -> [OK] Paginasi berhasil dibuat dengan %d tab.\n", pagination->total_tabs);
    return pagination;
}

/**
 * @brief Menampilkan ringkasan hasil pencarian.
 */
void displaySearchResult(const SearchResult *result)
{
    if (result == NULL || result->count == 0)
    {
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

    for (int i = 0; i < limit; i++)
    {
        Paper *p = result->papers[i];
        if (p)
        {
            // Check if strings are empty instead of NULL (since they're arrays)
            printf("[%d] %s\n", i + 1, (strlen(p->title) > 0) ? p->title : "No Title");
            printf("    Author    : %s\n", (strlen(p->authors) > 0) ? p->authors : "Unknown");
            printf("    Tahun     : %d\n", p->year);
            printf("    Sitasi    : %d\n", p->citation_count);
            printf("    Bidang    : %s\n\n", (strlen(p->field_of_study) > 0) ? p->field_of_study : "Unknown");
        }
    }
    if (result->count > limit)
    {
        printf("... dan %d hasil lainnya (siap untuk paginasi).\n", result->count - limit);
    }
    printf("======================================================\n");
}
void sort_papers_by_year(CitationNode** headRef, bool ascending)
{
    SortOrder order = ascending ? SORT_ASC : SORT_DESC;
    sortPaperListSLL(headRef, SORT_BY_YEAR, order);
}

/**
 * @brief Wrapper function untuk sort by citations
 * Digunakan oleh main_UI.c untuk kompatibilitas  
 */
void sort_papers_by_citations(CitationNode** headRef, bool ascending)
{
    SortOrder order = ascending ? SORT_ASC : SORT_DESC;
    sortPaperListSLL(headRef, SORT_BY_CITATIONS, order);
}


/**
 * @brief Membebaskan memori yang dialokasikan untuk SearchResult.
 */
void freeSearchResult(SearchResult *result)
{
    if (result != NULL)
    {
        if (result->papers)
        {
            free(result->papers);
        }
        free(result);
        printf(" -> [OK] Memori SearchResult telah dibebaskan.\n");
    }
}

//======================================================================
// BAGIAN 3: IMPLEMENTASI UTILITY FUNCTIONS
//======================================================================

/**
 * @brief Konversi enum SortCriteria ke string.
 */
const char *sortCriteriaToString(SortCriteria criteria)
{
    switch (criteria)
    {
    case SORT_BY_YEAR:
        return "year";
    case SORT_BY_CITATIONS:
        return "citations";
    default:
        return "unknown";
    }
}

/**
 * @brief Konversi enum SortOrder ke string.
 */
const char *sortOrderToString(SortOrder order)
{
    switch (order)
    {
    case SORT_ASC:
        return "ascending";
    case SORT_DESC:
        return "descending";
    default:
        return "unknown";
    }
}

/**
 * @brief Validasi enum SortCriteria.
 */
bool isValidSortCriteria(SortCriteria criteria)
{
    return (criteria == SORT_BY_YEAR || criteria == SORT_BY_CITATIONS);
}

/**
 * @brief Validasi enum SortOrder.
 */
bool isValidSortOrder(SortOrder order)
{
    return (order == SORT_ASC || order == SORT_DESC);
}

/**
 * @brief Menghitung total paper dalam hasil.
 */
int countPapersInResult(const SearchResult *result)
{
    return (result != NULL) ? result->count : 0;
}

/**
 * @brief Mengambil paper berdasarkan index dalam hasil.
 */
Paper *getPaperFromResult(const SearchResult *result, int index)
{
    if (result == NULL || index < 0 || index >= result->count)
    {
        return NULL;
    }
    return result->papers[index];
}

/**
 * @brief Fungsi demonstrasi untuk testing search dan sort.
 */
void demonstrateSearchAndSort(PaperLoader *loader)
{
    if (!loader)
    {
        printf("[Error] PaperLoader tidak valid untuk demonstrasi.\n");
        return;
    }

    printf("\n=== DEMONSTRASI SEARCH & SORT ENGINE ===\n");

    // Test 1: Search dan sort by year ascending
    printf("\n1. Test: Search 'Computer Science' + Sort by Year (Ascending)\n");
    SearchResult *result1 = performSearchAndSort(loader, "Computer Science", SORT_BY_YEAR, SORT_ASC);
    if (result1)
    {
        displaySearchResult(result1);

        // Test kirim ke pagination
        PaginationSystem *pagination1 = sendToPagination(result1);
        if (pagination1)
        {
            free_pagination_system(pagination1);
        }

        freeSearchResult(result1);
    }

    // Test 2: Search dan sort by citations descending
    printf("\n2. Test: Search 'Computer Science' + Sort by Citations (Descending)\n");
    SearchResult *result2 = performSearchAndSort(loader, "Computer Science", SORT_BY_CITATIONS, SORT_DESC);
    if (result2)
    {
        displaySearchResult(result2);
        freeSearchResult(result2);
    }

    printf("\n=== DEMONSTRASI SELESAI ===\n");
}