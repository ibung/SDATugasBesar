#include "../include/Rizky_DLLjsonParser.h"
#include <ctype.h>

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

void heapify_down(MaxHeap* heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    
    if (left < heap->size && 
        heap->papers[left]->citation_count > heap->papers[largest]->citation_count) {
        largest = left;
    }
    
    if (right < heap->size && 
        heap->papers[right]->citation_count > heap->papers[largest]->citation_count) {
        largest = right;
    }
    
    if (largest != index) {
        Paper* temp = heap->papers[index];
        heap->papers[index] = heap->papers[largest];
        heap->papers[largest] = temp;
        
        heapify_down(heap, largest);
    }
}

void free_heap(MaxHeap* heap) {
    if (heap == NULL) return;
    
    if (heap->papers != NULL) {
        free(heap->papers);
    }
    
    free(heap);
}

// Helper function to skip whitespace
char* skip_whitespace(char* str) {
    while (*str && isspace(*str)) {
        str++;
    }
    return str;
}

// Helper function to find the next occurrence of a character, considering escape sequences
char* find_next_char(char* str, char target) {
    while (*str) {
        if (*str == '\\') {
            str += 2; // Skip escaped character
            continue;
        }
        if (*str == target) {
            return str;
        }
        str++;
    }
    return NULL;
}

// Helper function to extract string value from JSON
int extract_json_string(const char* json, const char* key, char* output, int max_len) {
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);
    
    char* key_pos = strstr(json, search_pattern);
    if (!key_pos) return 0;
    
    // Move to the value part
    char* value_start = key_pos + strlen(search_pattern);
    value_start = skip_whitespace(value_start);
    
    if (*value_start != '"') return 0; // Not a string value
    
    value_start++; // Skip opening quote
    char* value_end = find_next_char(value_start, '"');
    if (!value_end) return 0;
    
    int len = value_end - value_start;
    if (len >= max_len) len = max_len - 1;
    
    strncpy(output, value_start, len);
    output[len] = '\0';
    
    return 1;
}

// Helper function to extract integer value from JSON
int extract_json_int(const char* json, const char* key, int* output) {
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);
    
    char* key_pos = strstr(json, search_pattern);
    if (!key_pos) return 0;
    
    // Move to the value part
    char* value_start = key_pos + strlen(search_pattern);
    value_start = skip_whitespace(value_start);
    
    *output = atoi(value_start);
    return 1;
}

// Helper function to extract array and count elements
int count_json_array_elements(const char* json, const char* key) {
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);
    
    char* key_pos = strstr(json, search_pattern);
    if (!key_pos) return 0;
    
    char* array_start = key_pos + strlen(search_pattern);
    array_start = skip_whitespace(array_start);
    
    if (*array_start != '[') return 0;
    
    array_start++; // Skip opening bracket
    array_start = skip_whitespace(array_start);
    
    if (*array_start == ']') return 0; // Empty array
    
    int count = 1; // At least one element
    int bracket_depth = 0;
    int in_string = 0;
    
    while (*array_start && (*array_start != ']' || bracket_depth > 0)) {
        if (*array_start == '\\' && in_string) {
            array_start += 2; // Skip escaped character
            continue;
        }
        
        if (*array_start == '"') {
            in_string = !in_string;
        } else if (!in_string) {
            if (*array_start == '[') {
                bracket_depth++;
            } else if (*array_start == ']') {
                bracket_depth--;
            } else if (*array_start == ',' && bracket_depth == 0) {
                count++;
            }
        }
        array_start++;
    }
    
    return count;
}

// Updated function to parse JSON file with the new format
int parse_json_file(const char* filename, Paper** papers_array, int max_papers) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }
    
    // Read entire file into memory
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* json_content = (char*)malloc(file_size + 1);
    if (!json_content) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return 0;
    }
    
    fread(json_content, 1, file_size, file);
    json_content[file_size] = '\0';
    fclose(file);
    
    int paper_count = 0;
    char* current_pos = json_content;
    
    // Find the start of the array
    char* array_start = strchr(current_pos, '[');
    if (!array_start) {
        printf("Error: Invalid JSON format - no array found\n");
        free(json_content);
        return 0;
    }
    
    current_pos = array_start + 1;
    
    // Parse each paper object
    while (paper_count < max_papers) {
        // Skip whitespace
        current_pos = skip_whitespace(current_pos);
        
        // Check for end of array
        if (*current_pos == ']') break;
        
        // Find the start of the next object
        if (*current_pos == ',') {
            current_pos++;
            current_pos = skip_whitespace(current_pos);
        }
        
        if (*current_pos != '{') {
            if (*current_pos == ']') break;
            current_pos++;
            continue;
        }
        
        // Find the end of this object
        int brace_count = 1;
        char* object_start = current_pos;
        char* object_end = current_pos + 1;
        int in_string = 0;
        
        while (*object_end && brace_count > 0) {
            if (*object_end == '\\' && in_string) {
                object_end += 2;
                continue;
            }
            
            if (*object_end == '"') {
                in_string = !in_string;
            } else if (!in_string) {
                if (*object_end == '{') {
                    brace_count++;
                } else if (*object_end == '}') {
                    brace_count--;
                }
            }
            object_end++;
        }
        
        // Extract paper data from this object
        int object_len = object_end - object_start;
        char* paper_json = (char*)malloc(object_len + 1);
        strncpy(paper_json, object_start, object_len);
        paper_json[object_len] = '\0';
        
        // Create new paper and extract data
        Paper* paper = (Paper*)malloc(sizeof(Paper));
        extract_paper_data(paper_json, paper);
        
        // Initialize paper's linked list fields
        paper->citations_head = NULL;
        paper->next = NULL;
        paper->prev = NULL;
        
        papers_array[paper_count] = paper;
        paper_count++;
        
        free(paper_json);
        current_pos = object_end;
    }
    
    free(json_content);
    printf("Successfully parsed %d papers from JSON file.\n", paper_count);
    return paper_count;
}

// Updated function to extract paper data from JSON object
void extract_paper_data(const char* json_line, Paper* paper) {
    // Initialize default values
    strcpy(paper->title, "Unknown Title");
    strcpy(paper->field_of_study, "Unknown Field");
    paper->year = 0;
    paper->citation_count = 0;
    
    // Extract title
    extract_json_string(json_line, "title", paper->title, sizeof(paper->title));
    
    // Extract year
    extract_json_int(json_line, "year", &paper->year);
    
    // Extract field of study (first field from fieldsOfStudy array)
    char* fields_start = strstr(json_line, "\"fieldsOfStudy\":");
    if (fields_start) {
        fields_start = strchr(fields_start, '[');
        if (fields_start) {
            fields_start++; // Skip opening bracket
            fields_start = skip_whitespace(fields_start);
            
            if (*fields_start == '"') {
                fields_start++; // Skip opening quote
                char* field_end = find_next_char(fields_start, '"');
                if (field_end) {
                    int field_len = field_end - fields_start;
                    if (field_len < sizeof(paper->field_of_study)) {
                        strncpy(paper->field_of_study, fields_start, field_len);
                        paper->field_of_study[field_len] = '\0';
                    }
                }
            }
        }
    }
    
    // Count citations from inCitations array
    paper->citation_count = count_json_array_elements(json_line, "inCitations");
    
    // Debug output
    printf("Parsed: %s | Field: %s | Year: %d | Citations: %d\n", 
           paper->title, paper->field_of_study, paper->year, paper->citation_count);
}