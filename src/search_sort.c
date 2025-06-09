#include "../include/search_sort.h"

Paper* create_paper(const char* title, const char* field, int year, int citations) {
    Paper* paper = (Paper*)malloc(sizeof(Paper));
    strcpy(paper->title, title);
    strcpy(paper->field_of_study, field);
    paper->year = year;
    paper->citation_count = citations;
    paper->citations_head = NULL;
    paper->next = NULL;
    paper->prev = NULL;
    return paper;
}

// Helper function untuk swap data antara dua paper nodes
void swap_paper_data(Paper* a, Paper* b) {
    // Swap title
    char temp_title[256];
    strcpy(temp_title, a->title);
    strcpy(a->title, b->title);
    strcpy(b->title, temp_title);
    
    // Swap field
    char temp_field[100];
    strcpy(temp_field, a->field_of_study);
    strcpy(a->field_of_study, b->field_of_study);
    strcpy(b->field_of_study, temp_field);
    
    // Swap year
    int temp_year = a->year;
    a->year = b->year;
    b->year = temp_year;
    
    // Swap citation count
    int temp_citations = a->citation_count;
    a->citation_count = b->citation_count;
    b->citation_count = temp_citations;
    
    // Note: We don't swap citations_head pointer as it should stay with original paper
}

void insert_paper_sorted_by_year(Paper** head, Paper* new_paper) {
    if (*head == NULL) {
        *head = new_paper;
        return;
    }
    
    if (new_paper->year < (*head)->year) {
        new_paper->next = *head;
        (*head)->prev = new_paper;
        *head = new_paper;
        return;
    }
    
    Paper* current = *head;
    while (current->next != NULL && current->next->year < new_paper->year) {
        current = current->next;
    }
    
    new_paper->next = current->next;
    if (current->next != NULL) {
        current->next->prev = new_paper;
    }
    current->next = new_paper;
    new_paper->prev = current;
}

void sort_by_citations(Paper** head) {
    if (*head == NULL || (*head)->next == NULL) return;
    
    // Bubble sort for simplicity (can be optimized)
    int swapped;
    Paper* ptr1;
    Paper* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = *head;
        
        while (ptr1->next != lptr) {
            if (ptr1->citation_count < ptr1->next->citation_count) {
                swap_paper_data(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

// Fungsi baru untuk sorting berdasarkan tahun (newest/oldest first)
void sort_papers_by_year(Paper** head, int newest_first) {
    if (*head == NULL || (*head)->next == NULL) return;
    
    int swapped;
    Paper* ptr1;
    Paper* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = *head;
        
        while (ptr1->next != lptr) {
            int should_swap = 0;
            
            if (newest_first) {
                // Newest first: swap if current year < next year
                should_swap = (ptr1->year < ptr1->next->year);
            } else {
                // Oldest first: swap if current year > next year
                should_swap = (ptr1->year > ptr1->next->year);
            }
            
            if (should_swap) {
                swap_paper_data(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

// Fungsi baru untuk sorting berdasarkan sitasi (most/least popular)
void sort_papers_by_citations(Paper** head, int most_popular_first) {
    if (*head == NULL || (*head)->next == NULL) return;
    
    int swapped;
    Paper* ptr1;
    Paper* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = *head;
        
        while (ptr1->next != lptr) {
            int should_swap = 0;
            
            if (most_popular_first) {
                // Most popular first: swap if current citations < next citations
                should_swap = (ptr1->citation_count < ptr1->next->citation_count);
            } else {
                // Least popular first: swap if current citations > next citations
                should_swap = (ptr1->citation_count > ptr1->next->citation_count);
            }
            
            if (should_swap) {
                swap_paper_data(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

void insert_paper_end(Paper** head, Paper* new_paper) {
    if (*head == NULL) {
        *head = new_paper;
        return;
    }
    
    Paper* current = *head;
    // Traverse ke akhir list
    while (current->next != NULL) {
        current = current->next;
    }
    
    // Insert di akhir
    current->next = new_paper;
    new_paper->prev = current;
    new_paper->next = NULL;
}

// Fungsi untuk menampilkan semua papers dalam list
void display_papers(Paper* head) {
    if (head == NULL) {
        printf("No papers found.\n");
        return;
    }
    
    printf("\n========== PAPERS LIST ==========\n");
    Paper* current = head;
    int count = 1;
    
    while (current != NULL) {
        printf("%d. Title: %s\n", count, current->title);
        printf("   Field: %s\n", current->field_of_study);
        printf("   Year: %d\n", current->year);
        printf("   Citations: %d\n", current->citation_count);
        printf("   --------------------------------\n");
        
        current = current->next;
        count++;
    }
    printf("Total papers: %d\n", count - 1);
    printf("=================================\n\n");
}

// Fungsi untuk menampilkan papers dengan format yang lebih compact
void display_papers_compact(Paper* head) {
    if (head == NULL) {
        printf("No papers found.\n");
        return;
    }
    
    printf("\n%-50s %-20s %-6s %-10s\n", "Title", "Field", "Year", "Citations");
    printf("================================================================================\n");
    
    Paper* current = head;
    int count = 1;
    
    while (current != NULL) {
        // Truncate title if too long
        char display_title[51];
        if (strlen(current->title) > 50) {
            strncpy(display_title, current->title, 47);
            strcat(display_title, "...");
        } else {
            strcpy(display_title, current->title);
        }
        
        printf("%-50s %-20s %-6d %-10d\n", 
               display_title, 
               current->field_of_study, 
               current->year, 
               current->citation_count);
        
        current = current->next;
        count++;
    }
    printf("================================================================================\n");
    printf("Total papers: %d\n\n", count - 1);
}

// Fungsi untuk membebaskan semua memori dalam linked list papers
void free_papers_list(Paper* head) {
    Paper* current = head;
    Paper* next_paper;
    
    while (current != NULL) {
        next_paper = current->next;
        
        // TODO: Free citations linked list jika ada
        // Ini tergantung implementasi struct Citation
        // Contoh: free_citations_list(current->citations_head);
        
        // Free current paper
        free(current);
        current = next_paper;
    }
}