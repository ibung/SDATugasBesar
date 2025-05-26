// File: heap_sorter.h
#ifndef HEAP_SORTER_H
#define HEAP_SORTER_H

#include <stdio.h>
#include <stdlib.h>
#include "Fatim_DLLPapers.h"

typedef struct {
    Paper** papers;
    int size;
    int capacity;
} MaxHeap;

MaxHeap* create_heap(int capacity);
void insert_heap(MaxHeap* heap, Paper* paper);
Paper* extract_max(MaxHeap* heap);
void heapify_down(MaxHeap* heap, int index);
void heapify_up(MaxHeap* heap, int index);
void free_heap(MaxHeap* heap);

// JSON Parser functions
int parse_json_file(const char* filename, Paper** papers_array, int max_papers);
void extract_paper_data(const char* json_line, Paper* paper);

#endif