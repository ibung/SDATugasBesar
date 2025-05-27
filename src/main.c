#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/umum.h"
#include "../include/Dzakir_BST.h"
#include "../include/Fatim_DLLPapers.h"
#include "../include/Ibnu_SLL.h"
#include "../include/Rizky_DLLjsonParser.h"
#include "../include/Abi_MainProgramStack.h"

BST_Node* field_index = NULL;
MenuStack* menu_history = NULL;
Paper* all_papers_head = NULL;  // Global list untuk semua papers

void display_main_menu() {
    printf("\n=== ACADEMIC PAPER SORTER ===\n");
    printf("1. Load data from JSON\n");
    printf("2. Search papers by field of study\n");
    printf("3. Display all fields\n");
    printf("4. Display all papers with sorting\n");
    printf("5. Show menu history\n");
    printf("6. Exit\n");
    printf("Choose option: ");
}

void display_sorting_menu() {
    printf("\n=== SORTING OPTIONS ===\n");
    printf("1. Sort by Year (Newest First)\n");
    printf("2. Sort by Year (Oldest First)\n");
    printf("3. Sort by Citations (Most Popular First)\n");
    printf("4. Sort by Citations (Least Popular First)\n");
    printf("5. No Sorting (Original Order)\n");
    printf("Choose sorting option: ");
}

void load_json_data() {
    printf("Enter JSON filename (or press Enter for 'data.json'): ");
    char filename[256];
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0; // Remove newline
    
    if (strlen(filename) == 0) {
        strcpy(filename, "../data/data.json");
    }
    
    printf("Loading data from %s...\n", filename);
    
    // Array untuk menampung papers dari JSON
    Paper* papers_array[1000];
    int paper_count = parse_json_file(filename, papers_array, 1000);
    
    if (paper_count == 0) {
        printf("No papers loaded or file not found.\n");
        return;
    }
    
    printf("Successfully loaded %d papers!\n", paper_count);
    
    // Clear existing data
    free_bst(field_index);
    free_papers_list(all_papers_head);
    field_index = NULL;
    all_papers_head = NULL;
    
    // Process loaded papers
    for (int i = 0; i < paper_count; i++) {
        Paper* paper = papers_array[i];
        
        // Add to global papers list
        insert_paper_end(&all_papers_head, paper);
        
        // Add field to BST if not exists
        BST_Node* field_node = search_field(field_index, paper->field_of_study);
        if (field_node == NULL) {
            field_index = insert_field(field_index, paper->field_of_study);
            field_node = search_field(field_index, paper->field_of_study);
        }
        
        // Add paper to field's paper list
        if (field_node != NULL) {
            Paper* field_paper = create_paper(paper->title, paper->field_of_study, 
                                            paper->year, paper->citation_count);
            insert_paper_end(&(field_node->papers_head), field_paper);
        }
    }
    
    printf("Data has been organized by fields.\n");
}

void search_by_field() {
    if (field_index == NULL) {
        printf("No data loaded. Please load JSON data first.\n");
        return;
    }
    
    char field[100];
    printf("Enter field of study: ");
    fgets(field, sizeof(field), stdin);
    field[strcspn(field, "\n")] = 0; // Remove newline
    
    BST_Node* node = search_field(field_index, field);
    if (node == NULL) {
        printf("Field '%s' not found.\n", field);
        return;
    }
    
    printf("\nPapers in field '%s':\n", field);
    
    // Get sorting preference
    display_sorting_menu();
    int sort_choice;
    scanf("%d", &sort_choice);
    getchar(); // consume newline
    
    // Create a copy of the papers list for sorting
    Paper* temp_head = NULL;
    Paper* current = node->papers_head;
    
    while (current != NULL) {
        Paper* new_paper = create_paper(current->title, current->field_of_study,
                                      current->year, current->citation_count);
        insert_paper_end(&temp_head, new_paper);
        current = current->next;
    }
    
    // Apply sorting
    switch (sort_choice) {
        case 1: // Year (Newest First)
            sort_papers_by_year(&temp_head, 1);
            break;
        case 2: // Year (Oldest First)
            sort_papers_by_year(&temp_head, 0);
            break;
        case 3: // Citations (Most Popular First)
            sort_by_citations(&temp_head);
            break;
        case 4: // Citations (Least Popular First)
            sort_papers_by_citations(&temp_head, 0);
            break;
        case 5: // No sorting
        default:
            break;
    }
    
    display_papers(temp_head);
    free_papers_list(temp_head);
}

void display_all_papers_with_sorting() {
    if (all_papers_head == NULL) {
        printf("No papers loaded. Please load JSON data first.\n");
        return;
    }
    
    display_sorting_menu();
    int sort_choice;
    scanf("%d", &sort_choice);
    getchar(); // consume newline
    
    // Create a copy for sorting
    Paper* temp_head = NULL;
    Paper* current = all_papers_head;
    
    while (current != NULL) {
        Paper* new_paper = create_paper(current->title, current->field_of_study,
                                      current->year, current->citation_count);
        insert_paper_end(&temp_head, new_paper);
        current = current->next;
    }
    
    // Apply sorting
    switch (sort_choice) {
        case 1: // Year (Newest First)
            sort_papers_by_year(&temp_head, 1);
            break;
        case 2: // Year (Oldest First)
            sort_papers_by_year(&temp_head, 0);
            break;
        case 3: // Citations (Most Popular First)
            sort_by_citations(&temp_head);
            break;
        case 4: // Citations (Least Popular First)
            sort_papers_by_citations(&temp_head, 0);
            break;
        case 5: // No sorting
        default:
            break;
    }
    
    display_papers(temp_head);
    free_papers_list(temp_head);
}

void initialize_sample_data() {
    printf("Initializing with sample data...\n");
    
    // Create sample papers
    Paper* paper1 = create_paper("Deep Learning in Computer Vision", "Computer Science", 2020, 150);
    Paper* paper2 = create_paper("Machine Learning Algorithms", "Computer Science", 2019, 200);
    Paper* paper3 = create_paper("DNA Sequencing Methods", "Biology", 2021, 75);
    Paper* paper4 = create_paper("Quantum Computing Applications", "Computer Science", 2022, 120);
    Paper* paper5 = create_paper("CRISPR Gene Editing", "Biology", 2020, 300);
    
    // Add to global list
    insert_paper_end(&all_papers_head, paper1);
    insert_paper_end(&all_papers_head, paper2);
    insert_paper_end(&all_papers_head, paper3);
    insert_paper_end(&all_papers_head, paper4);
    insert_paper_end(&all_papers_head, paper5);
    
    // Build field index
    field_index = insert_field(field_index, "Computer Science");
    field_index = insert_field(field_index, "Biology");
    
    // Add papers to respective fields
    BST_Node* cs_node = search_field(field_index, "Computer Science");
    if (cs_node != NULL) {
        insert_paper_end(&(cs_node->papers_head), create_paper("Deep Learning in Computer Vision", "Computer Science", 2020, 150));
        insert_paper_end(&(cs_node->papers_head), create_paper("Machine Learning Algorithms", "Computer Science", 2019, 200));
        insert_paper_end(&(cs_node->papers_head), create_paper("Quantum Computing Applications", "Computer Science", 2022, 120));
    }
    
    BST_Node* bio_node = search_field(field_index, "Biology");
    if (bio_node != NULL) {
        insert_paper_end(&(bio_node->papers_head), create_paper("DNA Sequencing Methods", "Biology", 2021, 75));
        insert_paper_end(&(bio_node->papers_head), create_paper("CRISPR Gene Editing", "Biology", 2020, 300));
    }
}

int main() {
    int choice;
    
    printf("=== ACADEMIC PAPER SORTER ===\n");
    printf("Welcome! You can start by loading JSON data or use sample data.\n");
    
    // Initialize with sample data
    initialize_sample_data();
    
    while (1) {
        display_main_menu();
        scanf("%d", &choice);
        getchar(); // consume newline
        
        switch (choice) {
            case 1:
                push_menu(&menu_history, 1, "Load JSON Data");
                load_json_data();
                break;
            case 2:
                push_menu(&menu_history, 2, "Search by Field");
                search_by_field();
                break;
            case 3:
                push_menu(&menu_history, 3, "Display Fields");
                printf("\nAvailable fields:\n");
                printf("==================\n");
                inorder_traversal(field_index);
                break;
            case 4:
                push_menu(&menu_history, 4, "Display All Papers");
                display_all_papers_with_sorting();
                break;
            case 5:
                printf("\nMenu History:\n");
                display_menu_history(menu_history);
                break;
            case 6:
                printf("Thank you for using Academic Paper Sorter!\n");
                free_bst(field_index);
                free_menu_stack(menu_history);
                free_papers_list(all_papers_head);
                exit(0);
            default:
                printf("Invalid option. Try again.\n");
        }
    }
    
    return 0;
}