#include "../include/Ibnu_SLL.h"

// Existing citation functions (unchanged)
Citation* create_citation(const char* paper_id, const char* title, int citation_count) {
    Citation* citation = (Citation*)malloc(sizeof(Citation));        
    strcpy(citation->citing_paper_id, paper_id);
    strcpy(citation->citing_paper_title, title);
    citation->citation_count = citation_count;
    citation->next = NULL;
    return citation;
}

void add_citation(Citation** head, Citation* new_citation) {
    if (*head == NULL) {
        *head = new_citation;
        return;
    }
    
    Citation* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_citation;
}

int count_citations(Citation* head) {
    int count = 0;
    Citation* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

Citation* merge_sort_citations(Citation* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    
    Citation* middle = get_middle(head);
    Citation* next_of_middle = middle->next;
    middle->next = NULL;
    
    Citation* left = merge_sort_citations(head);
    Citation* right = merge_sort_citations(next_of_middle);
    
    return merge_citations(left, right);
}

void display_citations(Citation* head) {
    if (head == NULL) {
        printf("No citations found.\n");
        return;
    }

    printf("\n========== CITATIONS LIST ==========\n");
    Citation* current = head;
    int count = 1;

    while (current != NULL) {
        printf("%d. Paper ID: %s\n", count, current->citing_paper_id);
        printf("   Title: %s\n", current->citing_paper_title);
        printf("   Citations: %d\n", current->citation_count);
        printf("   ----------------------------------\n");

        current = current->next;
        count++;
    }
    printf("Total citations: %d\n\n", count - 1);
}

void free_citations(Citation* head) {
    Citation* current = head;
    Citation* next_citation;
    
    while (current != NULL) {
        next_citation = current->next;
        free(current);
        current = next_citation;
    }
}

Citation* get_middle(Citation* head) {
    if (head == NULL) {
        return head;
    }
    
    Citation* slow = head;
    Citation* fast = head->next;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}

Citation* merge_citations(Citation* left, Citation* right) {
    Citation* result = NULL;

    if (left == NULL) {
        return right;
    }
    if (right == NULL) {
        return left;
    }

    if (left->citation_count >= right->citation_count) {
        result = left;
        result->next = merge_citations(left->next, right);
    } else {
        result = right;
        result->next = merge_citations(left, right->next);
    }

    return result;
}

// NEW PAGINATION FUNCTIONS

PaginationSystem* create_pagination_system() {
    PaginationSystem* pagination = (PaginationSystem*)malloc(sizeof(PaginationSystem));
    pagination->first_tab = NULL;
    pagination->last_tab = NULL;
    pagination->current_tab = NULL;
    pagination->total_tabs = 0;
    pagination->total_papers = 0;
    return pagination;
}

PaginationTab* create_pagination_tab(int tab_number) {
    PaginationTab* tab = (PaginationTab*)malloc(sizeof(PaginationTab));
    tab->tab_number = tab_number;
    tab->paper_count = 0;
    tab->next = NULL;
    tab->prev = NULL;
    
    // Initialize papers array to NULL
    for (int i = 0; i < 10; i++) {
        tab->papers[i] = NULL;
    }
    
    return tab;
}

void add_paper_to_pagination(PaginationSystem* pagination, Paper* paper) {
    // If no tabs exist or current last tab is full, create new tab
    if (pagination->last_tab == NULL || pagination->last_tab->paper_count >= 10) {
        PaginationTab* new_tab = create_pagination_tab(pagination->total_tabs + 1);
        
        if (pagination->first_tab == NULL) {
            // First tab
            pagination->first_tab = new_tab;
            pagination->last_tab = new_tab;
            pagination->current_tab = new_tab;
        } else {
            // Add to end of DLL
            new_tab->prev = pagination->last_tab;
            pagination->last_tab->next = new_tab;
            pagination->last_tab = new_tab;
        }
        pagination->total_tabs++;
    }
    
    // Add paper to current tab
    pagination->last_tab->papers[pagination->last_tab->paper_count] = paper;
    pagination->last_tab->paper_count++;
    pagination->total_papers++;
}

PaginationSystem* convert_papers_to_pagination(Paper** papers_array, int total_papers) {
    PaginationSystem* pagination = create_pagination_system();
    
    for (int i = 0; i < total_papers; i++) {
        add_paper_to_pagination(pagination, papers_array[i]);
    }
    
    return pagination;
}

void display_current_tab(PaginationSystem* pagination) {
    if (pagination->current_tab == NULL) {
        printf("No papers to display.\n");
        return;
    }
    
    PaginationTab* tab = pagination->current_tab;
    
    printf("\n==================== TAB %d ====================\n", tab->tab_number);
    printf("Showing papers %d-%d of %d total papers\n", 
           ((tab->tab_number - 1) * 10) + 1,
           ((tab->tab_number - 1) * 10) + tab->paper_count,
           pagination->total_papers);
    printf("====================================================\n");
    
    for (int i = 0; i < tab->paper_count; i++) {
        Paper* paper = tab->papers[i];
        printf("%d. Title: %s\n", ((tab->tab_number - 1) * 10) + i + 1, paper->title);
        printf("   Field: %s\n", paper->field_of_study);
        printf("   Year: %d\n", paper->year);
        printf("   Citations: %d\n", paper->citation_count);
        printf("   --------------------------------------------------\n");
    }
    
    printf("====================================================\n");
    printf("Tab %d of %d | Papers in this tab: %d\n", 
           tab->tab_number, pagination->total_tabs, tab->paper_count);
}

void display_pagination_interface(PaginationSystem* pagination) {
    if (pagination == NULL || pagination->current_tab == NULL) {
        printf("No data to display.\n");
        return;
    }
    
    while (1) {
        // Clear screen (optional)
        system("clear || cls");
        
        // Display current tab
        display_current_tab(pagination);
        
        // Display navigation options
        printf("\n========== NAVIGATION ==========\n");
        if (pagination->current_tab->prev != NULL) {
            printf("P - Previous Tab (%d)\n", pagination->current_tab->prev->tab_number);
        }
        if (pagination->current_tab->next != NULL) {
            printf("N - Next Tab (%d)\n", pagination->current_tab->next->tab_number);
        }
        printf("F - First Tab (1)\n");
        printf("L - Last Tab (%d)\n", pagination->total_tabs);
        printf("G - Go to specific tab\n");
        printf("Q - Quit pagination view\n");
        printf("================================\n");
        printf("Choice: ");
        
        char choice;
        scanf(" %c", &choice);
        getchar(); // consume newline
        
        switch (choice) {
            case 'P':
            case 'p':
                if (pagination->current_tab->prev != NULL) {
                    pagination->current_tab = pagination->current_tab->prev;
                } else {
                    printf("Already at first tab!\n");
                    printf("Press Enter to continue...");
                    getchar();
                }
                break;
                
            case 'N':
            case 'n':
                if (pagination->current_tab->next != NULL) {
                    pagination->current_tab = pagination->current_tab->next;
                } else {
                    printf("Already at last tab!\n");
                    printf("Press Enter to continue...");
                    getchar();
                }
                break;
                
            case 'F':
            case 'f':
                pagination->current_tab = pagination->first_tab;
                break;
                
            case 'L':
            case 'l':
                pagination->current_tab = pagination->last_tab;
                break;
                
            case 'G':
            case 'g': {
                printf("Enter tab number (1-%d): ", pagination->total_tabs);
                int tab_num;
                scanf("%d", &tab_num);
                getchar();
                
                if (tab_num >= 1 && tab_num <= pagination->total_tabs) {
                    // Navigate to specific tab
                    PaginationTab* target = pagination->first_tab;
                    for (int i = 1; i < tab_num; i++) {
                        target = target->next;
                    }
                    pagination->current_tab = target;
                } else {
                    printf("Invalid tab number! Press Enter to continue...");
                    getchar();
                }
                break;
            }
                
            case 'Q':
            case 'q':
                return;
                
            default:
                printf("Invalid choice! Press Enter to continue...");
                getchar();
                break;
        }
    }
}

void navigate_pagination(PaginationSystem* pagination) {
    display_pagination_interface(pagination);
}

void free_pagination_system(PaginationSystem* pagination) {
    if (pagination == NULL) return;
    
    PaginationTab* current = pagination->first_tab;
    while (current != NULL) {
        PaginationTab* next = current->next;
        free(current);
        current = next;
    }
    
    free(pagination);
}