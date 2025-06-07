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

