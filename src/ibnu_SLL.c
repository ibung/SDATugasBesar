#include "../include/Ibnu_SLL.h"

Citation* create_citation(const char* paper_id, const char* title) {
    Citation* citation = (Citation*)malloc(sizeof(Citation));
    strcpy(citation->citing_paper_id, paper_id);
    strcpy(citation->citing_paper_title, title);
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
        printf("   ----------------------------------\n");
        
        current = current->next;
        count++;
    }
    printf("Total citations: %d\n\n", count - 1);
}

// Fungsi untuk membebaskan semua memori dalam linked list citations
void free_citations(Citation* head) {
    Citation* current = head;
    Citation* next_citation;
    
    while (current != NULL) {
        next_citation = current->next;
        free(current);
        current = next_citation;
    }
}

// Fungsi untuk mendapatkan node tengah dari linked list (untuk merge sort)
Citation* get_middle(Citation* head) {
    if (head == NULL) {
        return head;
    }
    
    Citation* slow = head;
    Citation* fast = head->next;  // Start fast from next to get proper middle
    
    // Use two pointers: slow moves 1 step, fast moves 2 steps
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}

// Fungsi untuk menggabungkan dua sorted linked list (untuk merge sort)
Citation* merge_citations(Citation* left, Citation* right) {
    Citation* result = NULL;
    
    // Base cases
    if (left == NULL) {
        return right;
    }
    if (right == NULL) {
        return left;
    }
    
    // Compare titles and merge accordingly (alphabetical order)
    if (strcmp(left->citing_paper_title, right->citing_paper_title) <= 0) {
        result = left;
        result->next = merge_citations(left->next, right);
    } else {
        result = right;
        result->next = merge_citations(left, right->next);
    }
    
    return result;
}