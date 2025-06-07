#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../include/umum.h"
#include "../include/Dzakir_BST.h"
#include "../include/Fatim_DLLPapers.h"
#include "../include/Ibnu_SLL.h"
#include "../include/Ibnu_DLLPagination.h"
#include "../include/Rizky_DLLjsonParser.h"
#include "../include/Abi_MainProgramStack.h"

BST_Node* field_index = NULL;
MenuStack* menu_history = NULL;
Paper* all_papers_head = NULL;

// Function declarations
void generate_field_stats_recursive(BST_Node* root);
void display_sorting_menu();
void initialize_sample_data();
void load_json_data();
void display_json_data();
void display_all_papers_with_sorting();

void display_main_menu() {
    printf("\n=== ENHANCED ACADEMIC PAPER SORTER ===\n");
    printf("1. Display JSON data\n");
    printf("2. Search papers by field of study\n");
    printf("3. Display all fields\n");
    printf("4. Display all papers with sorting\n");
    printf("5. Get TOP N papers by citations (using MaxHeap)\n");
    printf("6. Analyze citations network\n");
    printf("7. Generate field statistics\n");
    printf("8. Advanced search with filters\n");
    printf("9. Show menu history\n");
    printf("10. Exit\n");
    printf("Choose option: ");
}

void display_sorting_menu() {
    printf("\n=== SORTING OPTIONS ===\n");
    printf("1. Sort by year (newest first)\n");
    printf("2. Sort by year (oldest first)\n");
    printf("3. Sort by citations (most cited first)\n");
    printf("4. Sort by citations (least cited first)\n");
    printf("Choose sorting option: ");
}

// Function untuk load data JSON saja (tanpa output)
void load_json_data() {
    char filename[256];
    strcpy(filename, "data/data.json");

    // Array to store parsed papers
    Paper* papers_array[1000]; // Adjust size as needed
    int paper_count = parse_json_file(filename, papers_array, 1000);
    
    if (paper_count == 0) {
        return; // Tidak ada output di sini
    }
    
    // Clear existing data
    free_papers_list(all_papers_head);
    free_bst(field_index);
    all_papers_head = NULL;
    field_index = NULL;
    
    // Process each parsed paper
    for (int i = 0; i < paper_count; i++) {
        Paper* paper = papers_array[i];
        
        // Add to main papers list
        insert_paper_end(&all_papers_head, paper);
        
        // Add field to BST if it doesn't exist
        field_index = insert_field(field_index, paper->field_of_study);
        
        // Find the field node and add paper to it
        BST_Node* field_node = search_field(field_index, paper->field_of_study);
        if (field_node) {
            // Create a copy for the field-specific list
            Paper* field_paper = create_paper(paper->title, paper->field_of_study, 
                                            paper->year, paper->citation_count);
            insert_paper_end(&field_node->papers_head, field_paper);
        }
    }
}

// Function untuk menampilkan data JSON
void display_json_data() {
    if (all_papers_head == NULL) {
        printf("No JSON data loaded. Please check if data.json exists and is properly formatted.\n");
        return;
    }
    
    // Count total papers
    int total_papers = 0;
    Paper* counter = all_papers_head;
    while (counter != NULL) {
        total_papers++;
        counter = counter->next;
    }
    
    printf("\n=== JSON DATA DISPLAY ===\n");
    printf("Successfully loaded %d papers from JSON file.\n", total_papers);
    printf("Papers have been organized by fields.\n\n");
    
    // Display all papers
    printf("All papers from JSON:\n");
    printf("====================\n");
    display_papers(all_papers_head);
    
    printf("\nData loading completed successfully!\n");
}

void display_all_papers_with_sorting() {
    if (all_papers_head == NULL) {
        printf("No papers loaded. Please load JSON data first or use sample data.\n");
        return;
    }
    
    printf("\n=== ALL PAPERS ===\n");
    
    display_sorting_menu();
    int sort_choice;
    scanf("%d", &sort_choice);
    getchar(); // consume newline
    
    // Create a temporary copy of the papers list for sorting
    Paper* temp_head = NULL;
    Paper* current = all_papers_head;
    
    while (current != NULL) {
        Paper* new_paper = create_paper(current->title, current->field_of_study,
                                      current->year, current->citation_count);
        insert_paper_end(&temp_head, new_paper);
        current = current->next;
    }
    
    // Apply sorting based on user choice
    switch (sort_choice) {
        case 1:
            sort_papers_by_year(&temp_head, 1); // newest first
            printf("\nPapers sorted by year (newest first):\n");
            break;
        case 2:
            sort_papers_by_year(&temp_head, 0); // oldest first
            printf("\nPapers sorted by year (oldest first):\n");
            break;
        case 3:
            sort_papers_by_citations(&temp_head, 1); // most cited first
            printf("\nPapers sorted by citations (most cited first):\n");
            break;
        case 4:
            sort_papers_by_citations(&temp_head, 0); // least cited first
            printf("\nPapers sorted by citations (least cited first):\n");
            break;
        default:
            printf("\nPapers in original order:\n");
            break;
    }
    
    display_papers(temp_head);
    
    // Free the temporary list
    free_papers_list(temp_head);
}

// NEW FUNCTION: Menggunakan MaxHeap untuk mendapatkan top N papers
void get_top_papers_by_citations() {
    if (all_papers_head == NULL) {
        printf("No papers loaded. Please load JSON data first.\n");
        return;
    }
    
    int top_n;
    printf("Enter number of top papers to display: ");
    scanf("%d", &top_n);
    getchar();
    
    // Count total papers first
    int total_papers = 0;
    Paper* counter = all_papers_head;
    while (counter != NULL) {
        total_papers++;
        counter = counter->next;
    }
    
    if (top_n > total_papers) {
        top_n = total_papers;
        printf("Adjusted to show all %d papers.\n", total_papers);
    }
    
    // Create heap with sufficient capacity
    MaxHeap* heap = create_heap(total_papers);
    
    // Insert all papers into heap
    Paper* current = all_papers_head;
    while (current != NULL) {
        insert_heap(heap, current);
        current = current->next;
    }
    
    printf("\n========== TOP %d PAPERS BY CITATIONS ==========\n", top_n);
    printf("%-5s %-50s %-20s %-6s %-10s\n", "Rank", "Title", "Field", "Year", "Citations");
    printf("===================================================================================\n");
    
    for (int i = 0; i < top_n && heap->size > 0; i++) {
        Paper* top_paper = extract_max(heap);
        
        // Truncate title if too long
        char display_title[51];
        if (strlen(top_paper->title) > 50) {
            strncpy(display_title, top_paper->title, 47);
            strcpy(display_title + 47, "...");
        } else {
            strcpy(display_title, top_paper->title);
        }
        
        printf("%-5d %-50s %-20s %-6d %-10d\n", 
               i+1, display_title, top_paper->field_of_study, 
               top_paper->year, top_paper->citation_count);
    }
    printf("===================================================================================\n\n");
    
    free_heap(heap);
}

// NEW FUNCTION: Menggunakan Citation SLL untuk analisis jaringan sitasi
void analyze_citations_network() {
    if (all_papers_head == NULL) {
        printf("No papers loaded. Please load JSON data first.\n");
        return;
    }
    
    printf("=== CITATIONS NETWORK ANALYSIS ===\n");
    
    // Create citations for demonstration (normally would be loaded from JSON)
    Citation* all_citations_head = NULL;
    
    // Sample citations creation (in real implementation, extract from JSON)
    Paper* current = all_papers_head;
    char citation_id[20];
    int id_counter = 1;
    
    while (current != NULL) {
        // Create sample citations for each paper
        if (current->citation_count > 0) {
            sprintf(citation_id, "ID_%d", id_counter);
            Citation* new_citation = create_citation(
                citation_id,
                current->title,
                current->citation_count
            );
            add_citation(&all_citations_head, new_citation);
            id_counter++;
        }
        current = current->next;
    }
    
    if (all_citations_head == NULL) {
        printf("No citations data available.\n");
        return;
    }
    
    printf("Original citations order:\n");
    display_citations(all_citations_head);
    
    printf("\nSorting citations by count (using merge sort)...\n");
    all_citations_head = merge_sort_citations(all_citations_head);
    
    printf("Sorted citations:\n");
    display_citations(all_citations_head);
    
    printf("Total citations in network: %d\n", count_citations(all_citations_head));
    
    free_citations(all_citations_head);
}

// NEW FUNCTION: Generate field statistics menggunakan BST traversal
void generate_field_statistics() {
    if (field_index == NULL) {
        printf("No data loaded. Please load JSON data first.\n");
        return;
    }
    
    printf("\n=== FIELD STATISTICS ===\n");
    printf("Fields and their paper counts:\n");
    printf("================================\n");
    
    // Modified inorder traversal with statistics
    generate_field_stats_recursive(field_index);
}

void generate_field_stats_recursive(BST_Node* root) {
    if (root != NULL) {
        generate_field_stats_recursive(root->left);
        
        // Count papers in this field
        int paper_count = 0;
        int total_citations = 0;
        int max_citations = 0;
        int min_citations = INT_MAX;
        
        Paper* current = root->papers_head;
        while (current != NULL) {
            paper_count++;
            total_citations += current->citation_count;
            if (current->citation_count > max_citations) {
                max_citations = current->citation_count;
            }
            if (current->citation_count < min_citations) {
                min_citations = current->citation_count;
            }
            current = current->next;
        }
        
        if (paper_count > 0) {
            double avg_citations = (double)total_citations / paper_count;
            printf("Field: %s\n", root->field);
            printf("  - Papers: %d\n", paper_count);
            printf("  - Total Citations: %d\n", total_citations);
            printf("  - Average Citations: %.2f\n", avg_citations);
            printf("  - Max Citations: %d\n", max_citations);
            printf("  - Min Citations: %d\n", min_citations == INT_MAX ? 0 : min_citations);
            printf("  ------------------------\n");
        }
        
        generate_field_stats_recursive(root->right);
    }
}

// NEW FUNCTION: Advanced search with multiple filters
void advanced_search_with_filters() {
    if (all_papers_head == NULL) {
        printf("No papers loaded. Please load JSON data first.\n");
        return;
    }
    
    printf("=== ADVANCED SEARCH ===\n");
    
    char field_filter[100] = "";
    int min_year = 0, max_year = 9999;
    int min_citations = 0, max_citations = INT_MAX;
    
    printf("Enter field filter (or press Enter to skip): ");
    fgets(field_filter, sizeof(field_filter), stdin);
    field_filter[strcspn(field_filter, "\n")] = 0;
    
    printf("Enter minimum year (or 0 to skip): ");
    scanf("%d", &min_year);
    
    printf("Enter maximum year (or 0 to skip): ");
    scanf("%d", &max_year);
    if (max_year == 0) max_year = 9999;
    
    printf("Enter minimum citations (or 0 to skip): ");
    scanf("%d", &min_citations);
    
    printf("Enter maximum citations (or 0 to skip): ");
    scanf("%d", &max_citations);
    if (max_citations == 0) max_citations = INT_MAX;
    
    getchar(); // consume newline
    
    // Apply filters and create filtered list
    Paper* filtered_head = NULL;
    Paper* current = all_papers_head;
    
    while (current != NULL) {
        int matches = 1;
        
        // Field filter
        if (strlen(field_filter) > 0 && 
            strstr(current->field_of_study, field_filter) == NULL) {
            matches = 0;
        }
        
        // Year filter
        if (current->year < min_year || current->year > max_year) {
            matches = 0;
        }
        
        // Citations filter
        if (current->citation_count < min_citations || 
            current->citation_count > max_citations) {
            matches = 0;
        }
        
        if (matches) {
            Paper* filtered_paper = create_paper(
                current->title, current->field_of_study,
                current->year, current->citation_count
            );
            insert_paper_end(&filtered_head, filtered_paper);
        }
        
        current = current->next;
    }
    
    if (filtered_head == NULL) {
        printf("No papers match the specified criteria.\n");
        return;
    }
    
    printf("\nFiltered results:\n");
    display_papers(filtered_head);
    
    free_papers_list(filtered_head);
}

// Enhanced search by field with detailed citation analysis
void enhanced_search_by_field() {
    if (field_index == NULL) {
        printf("No data loaded. Please load JSON data first.\n");
        return;
    }
    
    char field[100];
    printf("Enter field of study: ");
    fgets(field, sizeof(field), stdin);
    field[strcspn(field, "\n")] = 0;
    
    BST_Node* node = search_field(field_index, field);
    if (node == NULL) {
        printf("Field '%s' not found.\n", field);
        return;
    }
    
    printf("\n=== DETAILED ANALYSIS FOR FIELD: %s ===\n", field);
    
    // Option 1: Regular sorting
    printf("\n1. View with regular sorting\n");
    printf("2. View top N papers (using heap with pagination)\n");
    printf("3. View with citation network analysis\n");
    printf("Choose view option: ");
    
    int view_option;
    scanf("%d", &view_option);
    getchar();
    
    switch (view_option) {
        case 1: {
            // Regular sorting (existing functionality)
            display_sorting_menu();
            int sort_choice;
            scanf("%d", &sort_choice);
            getchar();
            
            Paper* temp_head = NULL;
            Paper* current = node->papers_head;
            
            while (current != NULL) {
                Paper* new_paper = create_paper(current->title, current->field_of_study,
                                              current->year, current->citation_count);
                insert_paper_end(&temp_head, new_paper);
                current = current->next;
            }
            
            switch (sort_choice) {
                case 1: sort_papers_by_year(&temp_head, 1); break;
                case 2: sort_papers_by_year(&temp_head, 0); break;
                case 3: sort_by_citations(&temp_head); break;
                case 4: sort_papers_by_citations(&temp_head, 0); break;
                default: break;
            }
            
            display_papers(temp_head);
            free_papers_list(temp_head);
            break;
        }
        
        case 2: {
            // Top N using heap with pagination
            int top_n;
            printf("Enter number of top papers: ");
            scanf("%d", &top_n);
            getchar();
            
            // Count total papers in this field first
            int total_papers_in_field = 0;
            Paper* counter = node->papers_head;
            while (counter != NULL) {
                total_papers_in_field++;
                counter = counter->next;
            }
            
            if (top_n > total_papers_in_field) {
                top_n = total_papers_in_field;
                printf("Adjusted to show all %d papers in this field.\n", total_papers_in_field);
            }
            
            // Create heap with sufficient capacity
            MaxHeap* heap = create_heap(total_papers_in_field);
            Paper* current = node->papers_head;
            
            // Insert all papers into heap
            while (current != NULL) {
                insert_heap(heap, current);
                current = current->next;
            }
            
            // Extract top N papers and store in array
            Paper** top_papers = (Paper**)malloc(top_n * sizeof(Paper*));
            for (int i = 0; i < top_n && heap->size > 0; i++) {
                top_papers[i] = extract_max(heap);
            }
            
            // Convert to pagination system
            PaginationSystem* pagination = convert_papers_to_pagination(top_papers, top_n);
            
            printf("\n=== TOP %d PAPERS IN %s (PAGINATED VIEW) ===\n", top_n, field);
            printf("Total tabs: %d (10 papers per tab)\n", pagination->total_tabs);
            printf("Use navigation to browse through tabs.\n");
            
            // Start pagination interface
            navigate_pagination(pagination);
            
            // Cleanup
            free_pagination_system(pagination);
            free(top_papers);
            free_heap(heap);
            break;
        }
        
        case 3: {
            // Citation network analysis for this field (unchanged)
            Citation* field_citations = NULL;
            Paper* current = node->papers_head;
            char citation_id[20];
            int id_counter = 1;
            
            while (current != NULL) {
                if (current->citation_count > 0) {
                    sprintf(citation_id, "FIELD_PAPER_%d", id_counter);
                    Citation* citation = create_citation(
                        citation_id, current->title, current->citation_count
                    );
                    add_citation(&field_citations, citation);
                    id_counter++;
                }
                current = current->next;
            }
            
            if (field_citations != NULL) {
                printf("Citations before sorting:\n");
                display_citations(field_citations);
                
                field_citations = merge_sort_citations(field_citations);
                
                printf("Citations after merge sort:\n");
                display_citations(field_citations);
                
                free_citations(field_citations);
            } else {
                printf("No citations data available for this field.\n");
            }
            break;
        }
    }
}

// Enhanced main function
int main() {
    int choice;
    
    printf("=== ENHANCED ACADEMIC PAPER SORTER ===\n");
    printf("Welcome! All features are now fully integrated.\n");
    
    // Automatically load JSON data at startup (silent)
    load_json_data();
    
    while (1) {
        display_main_menu();
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1:
                push_menu(&menu_history, 1, "Display JSON Data");
                display_json_data();
                break;
            case 2:
                push_menu(&menu_history, 2, "Enhanced Search by Field");
                enhanced_search_by_field();
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
                push_menu(&menu_history, 5, "Top N Papers (MaxHeap)");
                get_top_papers_by_citations();
                break;
            case 6:
                push_menu(&menu_history, 6, "Citations Network Analysis");
                analyze_citations_network();
                break;
            case 7:
                push_menu(&menu_history, 7, "Field Statistics");
                generate_field_statistics();
                break;
            case 8:
                push_menu(&menu_history, 8, "Advanced Search");
                advanced_search_with_filters();
                break;
            case 9:
                push_menu(&menu_history, 9, "Menu History");
                display_menu_history(menu_history);
                break;
            case 10:
                printf("Thank you for using Enhanced Academic Paper Sorter!\n");
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