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

// Fungsi untuk heapify down (menjaga properti max heap dari atas ke bawah)
void heapify_down(MaxHeap* heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    // Cari yang terbesar antara parent, left child, dan right child
    if (left < heap->size && 
        heap->papers[left]->citation_count > heap->papers[largest]->citation_count) {
        largest = left;
    }
    
    if (right < heap->size && 
        heap->papers[right]->citation_count > heap->papers[largest]->citation_count) {
        largest = right;
    }
    
    // Jika largest bukan parent, swap dan lanjutkan heapify
    if (largest != index) {
        Paper* temp = heap->papers[index];
        heap->papers[index] = heap->papers[largest];
        heap->papers[largest] = temp;
        
        heapify_down(heap, largest);
    }
}

// Fungsi untuk membebaskan memori heap
void free_heap(MaxHeap* heap) {
    if (heap == NULL) return;
    
    // Free array of paper pointers
    if (heap->papers != NULL) {
        free(heap->papers);
    }
    
    // Free heap structure
    free(heap);
}

// Fungsi untuk parsing file JSON dan mengisi array papers
int parse_json_file(const char* filename, Paper** papers_array, int max_papers) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }
    
    char line[1024];
    int paper_count = 0;
    
    while (fgets(line, sizeof(line), file) && paper_count < max_papers) {
        // Skip empty lines or lines that don't contain paper data
        if (strlen(line) < 10) continue;
        
        // Create new paper
        Paper* paper = (Paper*)malloc(sizeof(Paper));
        
        // Extract data from JSON line
        extract_paper_data(line, paper);
        
        // Initialize paper's linked list fields
        paper->citations_head = NULL;
        paper->next = NULL;
        paper->prev = NULL;
        
        papers_array[paper_count] = paper;
        paper_count++;
    }
    
    fclose(file);
    return paper_count;
}

// Fungsi untuk mengekstrak data paper dari baris JSON
void extract_paper_data(const char* json_line, Paper* paper) {
    // Initialize default values
    strcpy(paper->title, "Unknown Title");
    strcpy(paper->field_of_study, "Unknown Field");
    paper->year = 0;
    paper->citation_count = 0;
    
    // Simple JSON parsing (basic implementation)
    // Cari "title": "..."
    char* title_start = strstr(json_line, "\"title\":");
    if (title_start != NULL) {
        title_start = strchr(title_start, '"');
        if (title_start != NULL) {
            title_start = strchr(title_start + 1, '"');
            if (title_start != NULL) {
                title_start++; // Skip opening quote
                char* title_end = strchr(title_start, '"');
                if (title_end != NULL) {
                    int title_len = title_end - title_start;
                    if (title_len < 255) {
                        strncpy(paper->title, title_start, title_len);
                        paper->title[title_len] = '\0';
                    }
                }
            }
        }
    }
    
    // Cari "field_of_study": "..."
    char* field_start = strstr(json_line, "\"field_of_study\":");
    if (field_start != NULL) {
        field_start = strchr(field_start, '"');
        if (field_start != NULL) {
            field_start = strchr(field_start + 1, '"');
            if (field_start != NULL) {
                field_start++; // Skip opening quote
                char* field_end = strchr(field_start, '"');
                if (field_end != NULL) {
                    int field_len = field_end - field_start;
                    if (field_len < 99) {
                        strncpy(paper->field_of_study, field_start, field_len);
                        paper->field_of_study[field_len] = '\0';
                    }
                }
            }
        }
    }
    
    // Cari "year": value
    char* year_start = strstr(json_line, "\"year\":");
    if (year_start != NULL) {
        year_start += 7; // Skip "year":
        while (*year_start == ' ' || *year_start == '\t') year_start++; // Skip whitespace
        paper->year = atoi(year_start);
    }
    
    // Cari "citation_count": value
    char* citation_start = strstr(json_line, "\"citation_count\":");
    if (citation_start != NULL) {
        citation_start += 17; // Skip "citation_count":
        while (*citation_start == ' ' || *citation_start == '\t') citation_start++; // Skip whitespace
        paper->citation_count = atoi(citation_start);
    }
}