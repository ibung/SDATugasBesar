#include "../include/Fatim_DLLPapers.h"

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
                // Swap citations
                int temp = ptr1->citation_count;
                ptr1->citation_count = ptr1->next->citation_count;
                ptr1->next->citation_count = temp;
                
                // Swap titles and other data
                char temp_title[256];
                strcpy(temp_title, ptr1->title);
                strcpy(ptr1->title, ptr1->next->title);
                strcpy(ptr1->next->title, temp_title);
                
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