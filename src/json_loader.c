#include "../include/json_loader.h"
#include "../include/avl_paper_loader.h"
#include "../include/cJSON.h"

PaperLoader* initPaperLoader() {
    PaperLoader* loader = (PaperLoader*)malloc(sizeof(PaperLoader));
    loader->avlRoot = NULL;
    loader->citationManager = initCitationManager();
    loader->totalPapers = 0;
    loader->totalFields = 0;
    
    printf("Paper Loader berhasil diinisialisasi!\n");
    printf("Citation Manager terintegrasi\n");
    printf("AVL Tree siap untuk organisasi berdasarkan bidang studi\n\n");
    return loader;
}

void trimWhitespace(char* str) {
    int start = 0;
    int end = strlen(str) - 1;
    
    // Hapus whitespace di awal
    while (start <= end && isspace(str[start])) {
        start++;
    }
    
    // Hapus whitespace di akhir
    while (end >= start && isspace(str[end])) {
        end--;
    }
    
    // Copy string yang sudah dibersihkan
    int i;
    for (i = 0; i <= end - start; i++) {
        str[i] = str[start + i];
    }
    str[i] = '\0';
}

char* getJSONString(cJSON* json, const char* key, const char* defaultValue) {
    cJSON* item = cJSON_GetObjectItem(json, key);
    if (cJSON_IsString(item) && item->valuestring != NULL) {
        return item->valuestring;
    }
    return (char*)defaultValue;
}

int getJSONInt(cJSON* json, const char* key, int defaultValue) {
    cJSON* item = cJSON_GetObjectItem(json, key);
    if (cJSON_IsNumber(item)) {
        return item->valueint;
    }
    return defaultValue;
}

// FIXED: Ekstrak nama author pertama dari array authors
void extractFirstAuthor(cJSON* authorsArray, char* result) {
    if (!cJSON_IsArray(authorsArray) || cJSON_GetArraySize(authorsArray) == 0) {
        strcpy(result, "Unknown Author");
        return;
    }
    
    cJSON* firstAuthor = cJSON_GetArrayItem(authorsArray, 0);
    if (cJSON_IsObject(firstAuthor)) {
        char* name = getJSONString(firstAuthor, "name", "Unknown Author");
        strncpy(result, name, 99);
        result[99] = '\0';
    } else {
        strcpy(result, "Unknown Author");
    }
}

// FIXED: Ekstrak fields of study dari array
int extractFieldsOfStudy(cJSON* fieldsArray, char fields[][150]) {
    if (!cJSON_IsArray(fieldsArray)) {
        strcpy(fields[0], "General");
        return 1;
    }
    
    int count = 0;
    int arraySize = cJSON_GetArraySize(fieldsArray);
    
    for (int i = 0; i < arraySize && count < 5; i++) {
        cJSON* field = cJSON_GetArrayItem(fieldsArray, i);
        if (cJSON_IsString(field) && field->valuestring != NULL) {
            strncpy(fields[count], field->valuestring, 149);
            fields[count][149] = '\0';
            trimWhitespace(fields[count]); // Clean whitespace
            count++;
        }
    }
    
    // Jika tidak ada field yang valid, set default
    if (count == 0) {
        strcpy(fields[0], "General");
        count = 1;
    }
    
    return count;
}

// Tambahkan setelah fungsi extractFieldsOfStudy
int countCitations(cJSON* jsonObject) {
    cJSON* inCitationsArray = cJSON_GetObjectItem(jsonObject, "inCitations");
    
    if (inCitationsArray == NULL) {
        return 0;
    }
    
    if (!cJSON_IsArray(inCitationsArray)) {
        return 0;
    }
    
    return cJSON_GetArraySize(inCitationsArray);
}

// FIXED: Parse JSON object menjadi JSONPaper
JSONPaper* parseJSONPaper(cJSON* jsonObject) {
    if (jsonObject == NULL) {
        return NULL;
    }
    
    JSONPaper* paper = (JSONPaper*)malloc(sizeof(JSONPaper));
    if (paper == NULL) {
        return NULL;
    }
    
    // Extract basic fields
    strncpy(paper->id, getJSONString(jsonObject, "id", "unknown"), 99);
    paper->id[99] = '\0';
    
    strncpy(paper->title, getJSONString(jsonObject, "title", "Untitled Paper"), 299);
    paper->title[299] = '\0';
    
    // Extract authors
    cJSON* authorsArray = cJSON_GetObjectItem(jsonObject, "authors");
    extractFirstAuthor(authorsArray, paper->authors);
    
    // Extract year
    paper->year = getJSONInt(jsonObject, "year", 2000);
    
    // Extract fields of study
    cJSON* fieldsArray = cJSON_GetObjectItem(jsonObject, "fieldsOfStudy");
    paper->fieldCount = extractFieldsOfStudy(fieldsArray, paper->fieldsOfStudy);
    
    // FIXED: Extract citation count from inCitations array
    paper->citationCount = countCitations(jsonObject);
    
    return paper;
}

// FIXED: Load data dari file JSON Array format
int loadJSONData(PaperLoader* loader, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Tidak dapat membuka file %s\n", filename);
        return 0;
    }
    
    printf("Memuat data dari file: %s\n", filename);
    printf("Sedang parsing JSON Array dan membangun AVL Tree...\n\n");
    
    // Read entire file into buffer
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* json_buffer = (char*)malloc(file_size + 1);
    if (json_buffer == NULL) {
        printf("Error: Tidak dapat mengalokasi memory untuk file\n");
        fclose(file);
        return 0;
    }
    
    fread(json_buffer, 1, file_size, file);
    json_buffer[file_size] = '\0';
    fclose(file);
    
    // Parse JSON Array
    cJSON* json_array = cJSON_Parse(json_buffer);
    free(json_buffer);
    
    if (json_array == NULL) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return 0;
    }
    
    if (!cJSON_IsArray(json_array)) {
        printf("Error: JSON file harus berupa array\n");
        cJSON_Delete(json_array);
        return 0;
    }
    
    int array_size = cJSON_GetArraySize(json_array);
    int paperCount = 0;
    int processedEntries = 0;
    
    printf("Total papers dalam file: %d\n\n", array_size);
    
    // Process each paper in array
    for (int i = 0; i < array_size; i++) {
        cJSON* paper_object = cJSON_GetArrayItem(json_array, i);
        
        if (!cJSON_IsObject(paper_object)) {
            continue;
        }
        
        // Parse JSON object
        JSONPaper* jsonPaper = parseJSONPaper(paper_object);
        if (jsonPaper == NULL) continue;
        
        printf("Processing [%d/%d]: %.50s%s\n", 
               i + 1, array_size,
               jsonPaper->title,
               strlen(jsonPaper->title) > 50 ? "..." : "");
        
        // Process setiap field of study
        for (int j = 0; j < jsonPaper->fieldCount; j++) {
            Paper* newPaper = createPaper(
                jsonPaper->title,
                jsonPaper->authors,
                jsonPaper->year,
                jsonPaper->fieldsOfStudy[j],
                jsonPaper->citationCount
            );
            
            loader->avlRoot = insertToAVL(loader->avlRoot, newPaper);
            addCitation(loader->citationManager, newPaper);
            processedEntries++;
        }
        
        free(jsonPaper);
        paperCount++;
        
        // Progress indicator
        if (paperCount % 50 == 0) {
            printf("Processed %d papers (%d entries)...\n", 
                   paperCount, processedEntries);
        }
        
        // Limit untuk testing (hapus jika ingin load semua)
        if (paperCount >= 1000) {
            printf("Limiting to first 1000 papers for testing...\n");
            break;
        }
    }
    
    cJSON_Delete(json_array);
    
    loader->totalPapers = processedEntries;
    loader->totalFields = countAVLNodes(loader->avlRoot);
    
    printf("\nBerhasil memuat %d entries dari %d papers!\n", 
           processedEntries, paperCount);
    printf("Total bidang studi unik: %d\n\n", loader->totalFields);
    
    return paperCount;
}

// Utility function untuk cleanup memory
void freeJSONPaper(JSONPaper* paper) {
    if (paper != NULL) {
        free(paper);
    }
}