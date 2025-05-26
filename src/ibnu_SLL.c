#include "include/Ibnu_SLL.h"

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