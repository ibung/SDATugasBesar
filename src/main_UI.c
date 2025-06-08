#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/json_loader.h"
#include "include/avl_paper_loader.h"
#include "include/search_sort.h"
#include "include/pagination.h"
#include "include/citation_stack.h"

// Forward declarations
void mainMenu(PaperLoader* loader);
void searchByField(PaperLoader* loader);
void displayAllFields(PaperLoader* loader);
void displayAllPapersWithSorting(PaperLoader* loader);
void undoSearch(PaperLoader* loader);
Paper* convertCitationsToPaperList(CitationNode* citationHead, int* count);
void handleSortingOptions(Paper* paperList, int count);
void handlePagination(Paper* paperList, int count);

int main() {
    printf("=================================================\n");
    printf("🎓 ACADEMIC PAPER CITATION MANAGEMENT SYSTEM 🎓\n");
    printf("=================================================\n\n");
    
    // Initialize Paper Loader
    PaperLoader* loader = initPaperLoader();
    
    // Auto-load JSON data
    printf("⚡ Auto-loading JSON data...\n");
    if (loadJSONData(loader, "data/papers.json") == 0) {
        printf("❌ Gagal memuat data! Program akan keluar.\n");
        freePaperLoader(loader);
        return 1;
    }
    
    // Display main menu
    mainMenu(loader);
    
    // Cleanup
    freePaperLoader(loader);
    printf("\n👋 Terima kasih telah menggunakan sistem ini!\n");
    return 0;
}

void mainMenu(PaperLoader* loader) {
    int choice;
    
    while (1) {
        printf("\n🔍 ===== MAIN MENU =====\n");
        printf("1. 🔎 Search by Field of Study\n");
        printf("2. 📚 Display All Fields\n");
        printf("3. 📄 Display All Papers with Sorting\n");
        printf("4. ⏪ Undo Last Search\n");
        printf("5. 🚪 Exit\n");
        printf("========================\n");
        printf("Pilihan Anda: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("❌ Input tidak valid!\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }
        getchar(); // Consume newline
        
        switch (choice) {
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
                printf("🚪 Keluar dari program...\n");
                return;
            default:
                printf("❌ Pilihan tidak valid! Silakan pilih 1-5.\n");
                break;
        }
    }
}

void searchByField(PaperLoader* loader) {
    char fieldName[100];
    
    printf("\n🔍 SEARCH BY FIELD OF STUDY\n");
    printf("=============================\n");
    printf("Masukkan nama bidang studi: ");
    fgets(fieldName, sizeof(fieldName), stdin);
    fieldName[strcspn(fieldName, "\n")] = '\0'; // Remove newline
    
    // Find field in AVL Tree
    AVLNode* fieldNode = findFieldInAVL(loader->avlRoot, fieldName);
    
    if (fieldNode == NULL) {
        printf("❌ Bidang studi '%s' tidak ditemukan!\n", fieldName);
        
        // Push ke history even if not found
        char details[200];
        sprintf(details, "Search failed for field: %s", fieldName);
        pushHistory(loader->citationManager, "SEARCH_FIELD", details);
        return;
    }
    
    printf("✅ Ditemukan %d papers dalam bidang '%s'\n", 
           fieldNode->paperCount, fieldName);
    
    // Convert CitationNode list to Paper array
    int paperCount = 0;
    Paper* paperList = convertCitationsToPaperList(fieldNode->paperList, &paperCount);
    
    if (paperList == NULL) {
        printf("❌ Error dalam konversi data!\n");
        return;
    }
    
    // Push ke history
    char details[200];
    sprintf(details, "Found %d papers in field: %s", paperCount, fieldName);
    pushHistory(loader->citationManager, "SEARCH_FIELD", details);
    
    // Handle sorting options
    handleSortingOptions(paperList, paperCount);
    
    // Handle pagination after sorting
    handlePagination(paperList, paperCount);
    
    // Cleanup
    free(paperList);
}

void handleSortingOptions(Paper* paperList, int count) {
    int sortChoice;
    
    printf("\n📊 SORTING OPTIONS\n");
    printf("==================\n");
    printf("1. 📅 Sort by Year (Newest First)\n");
    printf("2. 📅 Sort by Year (Oldest First)\n");
    printf("3. 🏆 Sort by Citations (Most Popular)\n");
    printf("4. 🏆 Sort by Citations (Least Popular)\n");
    printf("==================\n");
    printf("Pilihan sorting: ");
    
    if (scanf("%d", &sortChoice) != 1) {
        printf("❌ Input tidak valid! Menggunakan urutan default.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();
    
    Paper* head = &paperList[0]; // Convert array to linked list head
    
    switch (sortChoice) {
        case 1:
            sort_papers_by_year(&head, 1); // Newest first
            printf("✅ Sorted by year (newest first)\n");
            break;
        case 2:
            sort_papers_by_year(&head, 0); // Oldest first
            printf("✅ Sorted by year (oldest first)\n");
            break;
        case 3:
            sort_papers_by_citations(&head, 1); // Most popular
            printf("✅ Sorted by citations (most popular)\n");
            break;
        case 4:
            sort_papers_by_citations(&head, 0); // Least popular
            printf("✅ Sorted by citations (least popular)\n");
            break;
        default:
            printf("❌ Pilihan tidak valid! Menggunakan urutan default.\n");
            break;
    }
}

void handlePagination(Paper* paperList, int count) {
    // Convert Paper array to pagination system
    Paper** papersArray = (Paper**)malloc(count * sizeof(Paper*));
    
    for (int i = 0; i < count; i++) {
        papersArray[i] = &paperList[i];
    }
    
    PaginationSystem* pagination = convert_papers_to_pagination(papersArray, count);
    
    printf("\n📖 Displaying results with pagination...\n");
    navigate_pagination(pagination);
    
    // Cleanup
    free(papersArray);
    free_pagination_system(pagination);
}

void displayAllFields(PaperLoader* loader) {
    int displayChoice;
    
    printf("\n📚 DISPLAY ALL FIELDS\n");
    printf("=====================\n");
    printf("1. 🔼 Ascending (A-Z)\n");
    printf("2. 🔽 Descending (Z-A)\n");
    printf("=====================\n");
    printf("Pilihan urutan: ");
    
    if (scanf("%d", &displayChoice) != 1) {
        printf("❌ Input tidak valid! Menggunakan ascending.\n");
        displayChoice = 1;
        while (getchar() != '\n');
    }
    getchar();
    
    printf("\n📊 FIELDS OF STUDY OVERVIEW\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    if (displayChoice == 2) {
        displayFieldsInOrderDescending(loader->avlRoot);
    } else {
        displayFieldsInOrderAscending(loader->avlRoot);
    }
    
    printf("════════════════════════════════════════════════════════════\n");
    printf("Total unique fields: %d\n", loader->totalFields);
    
    // Push ke history
    pushHistory(loader->citationManager, "VIEW_ALL_FIELDS", 
                displayChoice == 2 ? "Displayed fields (descending)" : "Displayed fields (ascending)");
}

void displayAllPapersWithSorting(PaperLoader* loader) {
    printf("\n📄 DISPLAY ALL PAPERS WITH SORTING\n");
    printf("===================================\n");
    
    // Convert all citations to paper list
    int totalCount = 0;
    Paper* allPapers = convertCitationsToPaperList(
        loader->citationManager->citationHead, &totalCount);
    
    if (allPapers == NULL) {
        printf("❌ Tidak ada data papers yang tersedia!\n");
        return;
    }
    
    printf("📊 Total papers available: %d\n", totalCount);
    
    // Handle sorting
    handleSortingOptions(allPapers, totalCount);
    
    // Handle pagination
    handlePagination(allPapers, totalCount);
    
    // Push ke history
    char details[100];
    sprintf(details, "Displayed all %d papers", totalCount);
    pushHistory(loader->citationManager, "VIEW_ALL_PAPERS", details);
    
    // Cleanup
    free(allPapers);
}

void undoSearch(PaperLoader* loader) {
    printf("\n⏪ UNDO LAST SEARCH\n");
    printf("===================\n");
    
    if (loader->citationManager->historyTop == NULL) {
        printf("❌ Tidak ada history pencarian untuk di-undo.\n");
        return;
    }
    
    // Display current history
    printf("📋 Current search history:\n");
    displayHistory(loader->citationManager);
    
    // Pop last action
    popHistory(loader->citationManager);
    
    printf("✅ Last search action has been undone.\n");
}

// Helper function to convert CitationNode linked list to Paper array
Paper* convertCitationsToPaperList(CitationNode* citationHead, int* count) {
    if (citationHead == NULL) {
        *count = 0;
        return NULL;
    }
    
    // Count total papers
    *count = 0;
    CitationNode* current = citationHead;
    while (current != NULL) {
        (*count)++;
        current = current->next;
    }
    
    // Allocate array
    Paper* paperList = (Paper*)malloc(*count * sizeof(Paper));
    if (paperList == NULL) return NULL;
    
    // Copy data
    current = citationHead;
    for (int i = 0; i < *count; i++) {
        if (current != NULL && current->paper != NULL) {
            // Copy paper data
            strcpy(paperList[i].title, current->paper->title);
            strcpy(paperList[i].authors, current->paper->authors);
            strcpy(paperList[i].field_of_study, current->paper->field_of_study);
            paperList[i].year = current->paper->year;
            paperList[i].citation_count = current->paper->citation_count;
            
            // Set up linked list pointers for sorting
            paperList[i].next = (i < *count - 1) ? &paperList[i + 1] : NULL;
            paperList[i].prev = (i > 0) ? &paperList[i - 1] : NULL;
            
            current = current->next;
        }
    }
    
    return paperList;
}