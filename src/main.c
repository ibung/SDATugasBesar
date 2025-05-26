#include "../include/Dzakir_BST.h"
#include "../include/Fatim_DLLPapers.h"
#include "../include/Ibnu_SLL.h"
#include "../include/Rizky_DLLjsonParser.h"
#include "../include/Abi_MainProgramStack.h"

BST_Node* field_index = NULL;
MenuStack* menu_history = NULL;

void display_main_menu() {
    printf("\n=== ACADEMIC PAPER SORTER ===\n");
    printf("1. Search papers by field of study\n");
    printf("2. Display all fields\n");
    printf("3. Load data from JSON\n");
    printf("4. Show menu history\n");
    printf("5. Exit\n");
    printf("Choose option: ");
}

void search_by_field() {
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
    printf("----------------------------------------\n");
    
    // Sort by citations using heap
    MaxHeap* heap = create_heap(1000);
    Paper* current = node->papers_head;
    
    while (current != NULL) {
        insert_heap(heap, current);
        current = current->next;
    }
    
    // Extract and display in descending order of citations
    while (heap->size > 0) {
        Paper* paper = extract_max(heap);
        printf("Title: %s\n", paper->title);
        printf("Year: %d\n", paper->year);
        printf("Citations: %d\n", paper->citation_count);
        printf("----------------------------------------\n");
    }
    
    free_heap(heap);
}

int main() {
    int choice;
    
    // Sample data initialization
    printf("Initializing Academic Paper Sorter...\n");
    
    // Create sample papers
    Paper* paper1 = create_paper("Deep Learning in Computer Vision", "Computer Science", 2020, 150);
    Paper* paper2 = create_paper("Machine Learning Algorithms", "Computer Science", 2019, 200);
    Paper* paper3 = create_paper("DNA Sequencing Methods", "Biology", 2021, 75);
    
    // Build field index
    field_index = insert_field(field_index, "Computer Science");
    field_index = insert_field(field_index, "Biology");
    
    // Add papers to respective fields
    BST_Node* cs_node = search_field(field_index, "Computer Science");
    if (cs_node != NULL) {
        insert_paper_end(&(cs_node->papers_head), paper1);
        insert_paper_end(&(cs_node->papers_head), paper2);
    }
    
    BST_Node* bio_node = search_field(field_index, "Biology");
    if (bio_node != NULL) {
        insert_paper_end(&(bio_node->papers_head), paper3);
    }
    
    while (1) {
        display_main_menu();
        scanf("%d", &choice);
        getchar(); // consume newline
        
        switch (choice) {
            case 1:
                push_menu(&menu_history, 1, "Search by Field");
                search_by_field();
                break;
            case 2:
                push_menu(&menu_history, 2, "Display Fields");
                printf("Available fields:\n");
                inorder_traversal(field_index);
                break;
            case 3:
                push_menu(&menu_history, 3, "Load JSON Data");
                printf("JSON loading feature coming soon...\n");
                break;
            case 4:
                printf("Menu History:\n");
                display_menu_history(menu_history);
                break;
            case 5:
                printf("Thank you for using Academic Paper Sorter!\n");
                free_bst(field_index);
                free_menu_stack(menu_history);
                exit(0);
            default:
                printf("Invalid option. Try again.\n");
        }
    }
    
    return 0;
}