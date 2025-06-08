#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paper_manager.h"

void displayMainMenu() {
    printf("\n========================================\n");
    printf("    SISTEM MANAJEMEN PAPER AKADEMIK\n");
    printf("========================================\n");
    printf("1. Searching by Field\n");
    printf("2. Display All Fields\n");
    printf("3. Display All Papers with Sorting\n");
    printf("4. Exit\n");
    printf("========================================\n");
    printf("Pilih menu (1-4): ");
}

void displaySearchMenu() {
    printf("\n========================================\n");
    printf("           SORTING OPTIONS\n");
    printf("========================================\n");
    printf("1. Sorting by Year (Newest)\n");
    printf("2. Sorting by Year (Oldest)\n");
    printf("3. Sorting by Citation (Popular)\n");
    printf("4. Sorting by Citation (Non Popular)\n");
    printf("5. Kembali ke Menu Utama\n");
    printf("========================================\n");
    printf("Pilih sorting (1-5): ");
}

void displayOrderMenu() {
    printf("\n========================================\n");
    printf("           ORDER OPTIONS\n");
    printf("========================================\n");
    printf("1. Ascending\n");
    printf("2. Descending\n");
    printf("========================================\n");
    printf("Pilih urutan (1-2): ");
}

int main() {
    PaperManager manager;
    int choice, sortChoice, orderChoice;
    char searchField[100];
    
    // Inisialisasi dan load data
    initPaperManager(&manager);
    
    printf("Loading data from JSON file...\n");
    if (loadPapersFromJSON(&manager, "papers.json") != 0) {
        printf("Error: Tidak dapat memuat data dari file JSON!\n");
        printf("Pastikan file 'papers.json' tersedia.\n");
        return 1;
    }
    
    printf("Data berhasil dimuat! Total papers: %d\n", manager.count);
    
    while (1) {
        displayMainMenu();
        scanf("%d", &choice);
        getchar(); // consume newline
        
        switch (choice) {
            case 1: // Searching by Field
                printf("\nMasukkan field pencarian (title/author/year): ");
                fgets(searchField, sizeof(searchField), stdin);
                searchField[strcspn(searchField, "\n")] = 0; // remove newline
                
                displaySearchMenu();
                scanf("%d", &sortChoice);
                
                if (sortChoice >= 1 && sortChoice <= 4) {
                    displayOrderMenu();
                    scanf("%d", &orderChoice);
                    
                    if (orderChoice >= 1 && orderChoice <= 2) {
                        searchAndDisplayPapers(&manager, searchField, sortChoice, orderChoice);
                    } else {
                        printf("Pilihan order tidak valid!\n");
                    }
                } else if (sortChoice != 5) {
                    printf("Pilihan sorting tidak valid!\n");
                }
                break;
                
            case 2: // Display All Fields
                displayAllFields(&manager);
                break;
                
            case 3: // Display All Papers with Sorting
                displaySearchMenu();
                scanf("%d", &sortChoice);
                
                if (sortChoice >= 1 && sortChoice <= 4) {
                    displayOrderMenu();
                    scanf("%d", &orderChoice);
                    
                    if (orderChoice >= 1 && orderChoice <= 2) {
                        displayAllPapersWithSorting(&manager, sortChoice, orderChoice);
                    } else {
                        printf("Pilihan order tidak valid!\n");
                    }
                } else if (sortChoice != 5) {
                    printf("Pilihan sorting tidak valid!\n");
                }
                break;
                
            case 4: // Exit
                printf("Terima kasih telah menggunakan sistem ini!\n");
                freePaperManager(&manager);
                return 0;
                
            default:
                printf("Pilihan tidak valid! Silakan pilih 1-4.\n");
        }
    }
    
    return 0;
}