#ifndef RIZKY_DLLJSONPARSER_H
#define RIZKY_DLLJSONPARSER_H

#include "umum.h"

typedef struct MaxHeap {
    Paper** papers;
    int size;
    int capacity;
} MaxHeap;

// Function declarations
MaxHeap* create_heap(int capacity);
void insert_heap(MaxHeap* heap, Paper* paper);
void heapify_up(MaxHeap* heap, int index);
Paper* extract_max(MaxHeap* heap);
void heapify_down(MaxHeap* heap, int index);
void free_heap(MaxHeap* heap);

// JSON parsing functions
char* skip_whitespace(char* str);
char* find_next_char(char* str, char target);
int extract_json_string(const char* json, const char* key, char* output, int max_len);
int extract_json_int(const char* json, const char* key, int* output);
int count_json_array_elements(const char* json, const char* key);
int parse_json_file(const char* filename, Paper** papers_array, int max_papers);
void extract_paper_data(const char* json_line, Paper* paper);

#endif