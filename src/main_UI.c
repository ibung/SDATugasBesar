#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/json_loader.h"
#include "../include/avl_paper_loader.h"
#include "../include/search_sort.h"
#include "../include/pagination.h"
#include "../include/citation_stack.h"

// Forward declarations
void mainMenu(PaperLoader *loader);
void searchByField(PaperLoader *loader);
void displayAllFields(PaperLoader *loader);
void displayAllPapersWithSorting(PaperLoader *loader);
void undoSearch(PaperLoader *loader);
Paper *convertCitationsToPaperList(CitationNode *citationHead, int *count);
void handleSortingOptions(Paper *paperList, int count);
void handlePagination(Paper *paperList, int count);
void clearScreen();
void pressEnterToContinue();

int main()
{
    printf("=======================================================\n");
    printf("ACADEMIC PAPER CITATION MANAGEMENT SYSTEM\n");
    printf("=======================================================\n");
    printf("Sistem Manajemen Sitasi Paper Akademik\n");
    printf("Pencarian berdasarkan Bidang Studi dengan AVL Tree\n");
    printf("Sorting dan Pagination untuk kemudahan navigasi\n");
    printf("=======================================================\n\n");

    // Initialize Paper Loader
    printf("Inisialisasi sistem...\n");
    PaperLoader *loader = initPaperLoader();

    // Auto-load JSON data
    printf("Auto-loading JSON data dari file papers.json...\n");
    printf("Mohon tunggu, sedang memproses data...\n\n");

    if (loadJSONData(loader, "data/data.json") == 0)
    {
        printf("GAGAL MEMUAT DATA!\n");
        printf("   Pastikan file 'data/data.json' tersedia\n");
        printf("   Program akan keluar...\n");
        freePaperLoader(loader);
        pressEnterToContinue();
        return 1;
    }

    printf("Data berhasil dimuat!\n");
    printf("Total papers: %d\n", loader->totalPapers);
    printf("Total fields: %d\n\n", loader->totalFields);

    // Display main menu
    mainMenu(loader);

    // Cleanup
    freePaperLoader(loader);
    printf("\n============================================\n");
    printf("Terima kasih telah menggunakan sistem ini!\n");
    printf("Academic Paper Management System - Keluar\n");
    printf("============================================\n");

    return 0;
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue()
{
    printf("\nPress Enter to continue...");
    while (getchar() != '\n')
        ;
    getchar();
}

void mainMenu(PaperLoader *loader)
{
    int choice;

    while (1)
    {
        clearScreen();
        printf("+--------------------------------------------------+\n");
        printf("|           ACADEMIC PAPER MANAGEMENT              |\n");
        printf("+--------------------------------------------------+\n");
        printf("| Total Papers: %-6d | Total Fields: %-6d          |\n",
               loader->totalPapers, loader->totalFields);
        printf("+--------------------------------------------------+\n");
        printf("|                  MAIN MENU                       |\n");
        printf("| 1. Search Papers by Field of Study               |\n");
        printf("|    - Pencarian berdasarkan bidang studi          |\n");
        printf("| 2. Display All Fields Overview                   |\n");
        printf("|    - Lihat semua bidang studi                    |\n");
        printf("| 3. Display All Papers with Sorting               |\n");
        printf("|    - Tampilkan semua paper dengan sorting        |\n");
        printf("| 4. Undo Last Search Operation                    |\n");
        printf("|    - Batalkan operasi pencarian terakhir         |\n");
        printf("| 5. Exit Program                                  |\n");
        printf("|    - Keluar dari sistem                          |\n");
        printf("+--------------------------------------------------+\n");

        if (scanf("%d", &choice) != 1)
        {
            printf("Input tidak valid! Harap masukkan angka 1-5.\n");
            while (getchar() != '\n')
                ; // Clear input buffer
            pressEnterToContinue();
            continue;
        }
        getchar(); // Consume newline

        switch (choice)
        {
        case 1:
            searchByField(loader);
            break;
        case 2:
            displayAllFields(loader);
            break;
        case 3:
            displayAllPapersWithSorting(loader);
            break;
        case 4:
            undoSearch(loader);
            break;
        case 5:
            printf("\nKeluar dari program...\n");
            printf("Membersihkan data dari memory...\n");
            return;
        default:
            printf("Pilihan tidak valid! Silakan pilih 1-5.\n");
            pressEnterToContinue();
            break;
        }
    }
}

void searchByField(PaperLoader *loader)
{
    char fieldName[100];

    clearScreen();
    printf("+==========================================================+\n");
    printf("|                SEARCH BY FIELD OF STUDY                  |\n");
    printf("+==========================================================+\n");
    printf("\nContoh bidang studi yang tersedia:\n");
    printf("   • Computer Science\n");
    printf("   • Mathematics\n");
    printf("   • Physics\n");
    printf("   • Engineering\n");
    printf("   • Biology\n");
    printf("   • dll.\n\n");

    printf("Masukkan nama bidang studi: ");
    fgets(fieldName, sizeof(fieldName), stdin);
    fieldName[strcspn(fieldName, "\n")] = '\0'; // Remove newline

    if (strlen(fieldName) == 0)
    {
        printf("Nama bidang studi tidak boleh kosong!\n");
        pressEnterToContinue();
        return;
    }

    printf("\nMencari papers dalam bidang '%s'...\n", fieldName);

    // Find field in AVL Tree
    AVLNode *fieldNode = findFieldInAVL(loader->avlRoot, fieldName);

    if (fieldNode == NULL)
    {
        printf("Bidang studi '%s' tidak ditemukan!\n", fieldName);
        printf("Tips: Pastikan penulisan bidang studi benar dan case-sensitive\n");

        // Push ke history even if not found
        char details[200];
        snprintf(details, sizeof(details), "Search failed for field: %s", fieldName);
        pushHistory(loader->citationManager, "SEARCH_FIELD", details);

        pressEnterToContinue();
        return;
    }

    printf("Ditemukan %d papers dalam bidang '%s'\n",
           fieldNode->paperCount, fieldName);
    printf("Memproses data untuk sorting dan pagination...\n\n");

    // Convert CitationNode list to Paper array
    int paperCount = 0;
    Paper *paperList = convertCitationsToPaperList(fieldNode->paperList, &paperCount);

    if (paperList == NULL)
    {
        printf("Error dalam konversi data!\n");
        pressEnterToContinue();
        return;
    }

    // Push ke history
    char details[200];
    snprintf(details, sizeof(details), "Found %d papers in field: %s", paperCount, fieldName);
    pushHistory(loader->citationManager, "SEARCH_FIELD", details);

    // Handle sorting options
    handleSortingOptions(paperList, paperCount);

    // Handle pagination after sorting
    handlePagination(paperList, paperCount);

    // Cleanup
    free(paperList);
}

void handleSortingOptions(Paper *paperList, int count)
{
    int sortChoice;

    printf("+--------------------------------------------------------+\n");
    printf("|                  SORTING OPTIONS                       |\n");
    printf("+--------------------------------------------------------+\n");
    printf("|  Pilih metode pengurutan data:                         |\n");
    printf("|                                                        |\n");
    printf("|  1. Sort by Year (Newest First) - Terbaru              |\n");
    printf("|     -> Urutkan dari tahun terbaru ke terlama           |\n");
    printf("|                                                        |\n");
    printf("|  2. Sort by Year (Oldest First) - Terlama              |\n");
    printf("|     -> Urutkan dari tahun terlama ke terbaru           |\n");
    printf("|                                                        |\n");
    printf("|  3. Sort by Citations (Most Popular) - Populer         |\n");
    printf("|     -> Urutkan dari yang paling banyak dikutip         |\n");
    printf("|                                                        |\n");
    printf("|  4. Sort by Citations (Least Popular) - Kurang         |\n");
    printf("|     -> Urutkan dari yang paling sedikit dikutip        |\n");
    printf("|                                                        |\n");
    printf("|  5. Default Order - Tanpa pengurutan                   |\n");
    printf("|     -> Tampilkan sesuai urutan asli                    |\n");
    printf("+--------------------------------------------------------+\n");
    printf("\nPilihan sorting (1-5): ");

    if (scanf("%d", &sortChoice) != 1)
    {
        printf("Input tidak valid! Menggunakan urutan default.\n");
        while (getchar() != '\n')
            ;
        pressEnterToContinue();
        return;
    }
    getchar();

    // Validate count
    if (count <= 0)
    {
        printf("Tidak ada data untuk diurutkan!\n");
        pressEnterToContinue();
        return;
    }

    // Convert array to linked list head for sorting functions
    Paper *head = &paperList[0];

    printf("\nMelakukan sorting...");
    fflush(stdout);

    switch (sortChoice)
    {
    case 1:
        sort_papers_by_year(&head, 1); // Newest first
        printf(" Sorted by year (newest first)\n");
        break;
    case 2:
        sort_papers_by_year(&head, 0); // Oldest first
        printf(" Sorted by year (oldest first)\n");
        break;
    case 3:
        sort_papers_by_citations(&head, 1); // Most popular
        printf(" Sorted by citations (most popular)\n");
        break;
    case 4:
        sort_papers_by_citations(&head, 0); // Least popular
        printf(" Sorted by citations (least popular)\n");
        break;
    case 5:
    default:
        printf(" Using default order\n");
        break;
    }

    printf("Data siap untuk ditampilkan dengan pagination\n");
}

void handlePagination(Paper *paperList, int count)
{
    printf("\nMempersiapkan pagination system...\n");

    // Convert Paper array to pagination system
    Paper **papersArray = (Paper **)malloc(count * sizeof(Paper *));
    if (papersArray == NULL)
    {
        printf("Error: Tidak dapat mengalokasi memory untuk pagination\n");
        pressEnterToContinue();
        return;
    }

    for (int i = 0; i < count; i++)
    {
        papersArray[i] = &paperList[i];
    }

    PaginationSystem *pagination = convert_papers_to_pagination(papersArray, count);
    if (pagination == NULL)
    {
        printf("Error: Gagal membuat pagination system\n");
        free(papersArray);
        pressEnterToContinue();
        return;
    }

    printf("Pagination ready! Total %d pages (10 papers per page)\n", pagination->total_tabs);
    printf("Gunakan navigation controls untuk browse papers\n");
    pressEnterToContinue();

    // Navigate through pagination
    navigate_pagination(pagination);

    // Cleanup
    free(papersArray);
    free_pagination_system(pagination);
}

void displayAllFields(PaperLoader *loader)
{
    int displayChoice;

    clearScreen();
    printf("+-----------------------------------------------------------+\n");
    printf("|                DISPLAY ALL FIELDS                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  Pilih urutan tampilan bidang studi:                      |\n");
    printf("|                                                           |\n");
    printf("|  1. Ascending (A-Z) - Alfabetis Naik                      |\n");
    printf("|     -> Tampilkan dari A sampai Z                          |\n");
    printf("|                                                           |\n");
    printf("|  2. Descending (Z-A) - Alfabetis Turun                    |\n");
    printf("|     -> Tampilkan dari Z sampai A                          |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("\nPilihan urutan (1-2): ");

    if (scanf("%d", &displayChoice) != 1)
    {
        printf("Input tidak valid! Menggunakan ascending (A-Z).\n");
        displayChoice = 1;
        while (getchar() != '\n')
            ;
    }
    getchar();

    clearScreen();
    printf("+=============================================================================+\n");
    printf("|                        FIELDS OF STUDY OVERVIEW                             |\n");
    printf("+-----------------------------------------------------------------------------+\n");
    printf("| Field Name                    | Papers | Height | Balance | AVL Info        |\n");
    printf("+=============================================================================+\n");

    if (displayChoice == 2)
    {
        displayFieldsInOrderDescending(loader->avlRoot);
    }
    else
    {
        displayFieldsInOrderAscending(loader->avlRoot);
    }

    printf("+=============================================================================+\n");
    printf("| STATISTIK:                                                                  |\n");
    printf("|   - Total unique fields : %-3d                                              |\n", loader->totalFields);
    printf("|   - Total papers loaded  : %-4d                                             |\n", loader->totalPapers);
    printf("|   - AVL Tree structure   : Balanced untuk pencarian optimal                 |\n");
    printf("+=============================================================================+\n");

    // Push ke history
    pushHistory(loader->citationManager, "VIEW_ALL_FIELDS",
                displayChoice == 2 ? "Displayed fields (descending)" : "Displayed fields (ascending)");

    pressEnterToContinue();
}

void displayAllPapersWithSorting(PaperLoader *loader)
{
    clearScreen();
    printf("+==========================================================+\n");
    printf("|           DISPLAY ALL PAPERS WITH SORTING                |\n");
    printf("+==========================================================+\n");
    printf("\nMengumpulkan semua papers dari database...\n");

    // Convert all citations to paper list
    int totalCount = 0;
    Paper *allPapers = convertCitationsToPaperList(
        loader->citationManager->citationHead, &totalCount);

    if (allPapers == NULL)
    {
        printf("Tidak ada data papers yang tersedia!\n");
        printf("Pastikan data JSON telah dimuat dengan benar\n");
        pressEnterToContinue();
        return;
    }

    printf("Berhasil mengumpulkan %d papers dari semua bidang studi\n", totalCount);
    printf("Siap untuk sorting dan pagination\n\n");

    // Handle sorting
    handleSortingOptions(allPapers, totalCount);

    // Handle pagination
    handlePagination(allPapers, totalCount);

    // Push ke history
    char details[100];
    snprintf(details, sizeof(details), "Displayed all %d papers", totalCount);
    pushHistory(loader->citationManager, "VIEW_ALL_PAPERS", details);

    // Cleanup
    free(allPapers);
}

void undoSearch(PaperLoader *loader)
{
    clearScreen();
    printf("+==========================================================+\n");
    printf("|                UNDO LAST SEARCH                          |\n");
    printf("+==========================================================+\n");

    if (loader->citationManager->historyTop == NULL)
    {
        printf("Tidak ada history pencarian untuk di-undo.\n");
        printf("Lakukan beberapa operasi pencarian terlebih dahulu\n");
        pressEnterToContinue();
        return;
    }

    printf("Current search history:\n");

    // Display current history
    displayHistory(loader->citationManager);

    printf("\nMelakukan undo operasi terakhir...\n");

    // Pop last action
    popHistory(loader->citationManager);

    printf("Last search action has been undone successfully!\n");
    printf("History telah diperbarui\n");

    pressEnterToContinue();
}

// Helper function to convert CitationNode linked list to Paper array
Paper *convertCitationsToPaperList(CitationNode *citationHead, int *count)
{
    if (citationHead == NULL)
    {
        *count = 0;
        return NULL;
    }

    // Count total papers
    *count = 0;
    CitationNode *current = citationHead;
    while (current != NULL)
    {
        (*count)++;
        current = current->next;
    }

    // Allocate array
    Paper *paperList = (Paper *)malloc(*count * sizeof(Paper));
    if (paperList == NULL)
        return NULL;

    // Copy data with proper error checking
    current = citationHead;
    for (int i = 0; i < *count; i++)
    {
        if (current != NULL && current->paper != NULL)
        {
            // Copy paper data with correct member names
            strncpy(paperList[i].title, current->paper->title, sizeof(paperList[i].title) - 1);
            paperList[i].title[sizeof(paperList[i].title) - 1] = '\0';

            strncpy(paperList[i].authors, current->paper->authors, sizeof(paperList[i].authors) - 1);
            paperList[i].authors[sizeof(paperList[i].authors) - 1] = '\0';

            strncpy(paperList[i].field_of_study, current->paper->field_of_study, sizeof(paperList[i].field_of_study) - 1);
            paperList[i].field_of_study[sizeof(paperList[i].field_of_study) - 1] = '\0';

            paperList[i].year = current->paper->year;
            paperList[i].citations = current->paper->citations;

            // Set up linked list pointers for sorting
            paperList[i].next = (i < *count - 1) ? &paperList[i + 1] : NULL;
            paperList[i].prev = (i > 0) ? &paperList[i - 1] : NULL;

            current = current->next;
        }
    }

    return paperList;
}