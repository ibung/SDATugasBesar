#include "../include/Rizky_DLLjsonParser.h"

MaxHeap* create_heap(int capacity) {
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    heap->papers = (Paper**)malloc(capacity * sizeof(Paper*));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void insert_heap(MaxHeap* heap, Paper* paper) {
    if (heap->size >= heap->capacity) return;
    
    heap->papers[heap->size] = paper;
    heapify_up(heap, heap->size);
    heap->size++;
}

void heapify_up(MaxHeap* heap, int index) {
    if (index == 0) return;
    
    int parent = (index - 1) / 2;
    if (heap->papers[index]->citation_count > heap->papers[parent]->citation_count) {
        // Swap
        Paper* temp = heap->papers[index];
        heap->papers[index] = heap->papers[parent];
        heap->papers[parent] = temp;
        
        heapify_up(heap, parent);
    }
}

Paper* extract_max(MaxHeap* heap) {
    if (heap->size <= 0) return NULL;
    
    Paper* max_paper = heap->papers[0];
    heap->papers[0] = heap->papers[heap->size - 1];
    heap->size--;
    heapify_down(heap, 0);
    
    return max_paper;
}
