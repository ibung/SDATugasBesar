#include "../include/json_loader.h"
#include "../include/cJSON.h"


PaperLoader* initPaperLoader() {
    PaperLoader* loader = (PaperLoader*)malloc(sizeof(PaperLoader));
    loader->avlRoot = NULL;
    loader->citationManager = initCitationManager();
    loader->totalPapers = 0;
    loader->totalFields = 0;
    
    printf("🚀 berhasil diinisialisasi!\n");
    printf("📚 Citation Manager terintegrasi\n");
    printf("🌳 AVL Tree siap untuk organisasi berdasarkan bidang studi\n\n");
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

// Fungsi helper untuk mengambil integer dari cJSON object
int getJSONInt(cJSON* json, const char* key, int defaultValue) {
    cJSON* item = cJSON_GetObjectItem(json, key);
    if (cJSON_IsNumber(item)) {
        return item->valueint;
    }
    return defaultValue;
}

// Ekstrak nama author pertama dari array authors
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

// Ekstrak fields of study dari array
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

// Parse JSON line menjadi JSONPaper menggunakan cJSON
JSONPaper* parseJSONPaper(const char* jsonLine) {
    cJSON* json = cJSON_Parse(jsonLine);
    if (json == NULL) {
        printf("❌ Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }
    
    JSONPaper* paper = (JSONPaper*)malloc(sizeof(JSONPaper));
    if (paper == NULL) {
        cJSON_Delete(json);
        return NULL;
    }
    
    // Extract basic fields menggunakan helper functions
    strncpy(paper->id, getJSONString(json, "id", "unknown"), 99);
    paper->id[99] = '\0';
    
    strncpy(paper->title, getJSONString(json, "title", "Untitled Paper"), 299);
    paper->title[299] = '\0';
    
    // Extract authors
    cJSON* authorsArray = cJSON_GetObjectItem(json, "authors");
    extractFirstAuthor(authorsArray, paper->authors);
    
    // Extract year
    paper->year = getJSONInt(json, "year", 2000);
    
    // Extract fields of study
    cJSON* fieldsArray = cJSON_GetObjectItem(json, "fieldsOfStudy");
    paper->fieldCount = extractFieldsOfStudy(fieldsArray, paper->fieldsOfStudy);
    
    // Extract citation count (coba beberapa nama field)
    paper->citationCount = getJSONInt(json, "n_citation", 0);
    if (paper->citationCount == 0) {
        paper->citationCount = getJSONInt(json, "citationCount", 0);
    }
    
    cJSON_Delete(json);
    return paper;
}

// Load data dari file JSON (versi yang disederhanakan)
int loadJSONData(PaperLoader* loader, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("❌ Error: Tidak dapat membuka file %s\n", filename);
        return 0;
    }
    
    printf("📖 Memuat data dari file: %s\n", filename);
    printf("⏳ Sedang parsing JSON dan membangun AVL Tree...\n\n");
    
    char line[4096];
    int paperCount = 0;
    int processedEntries = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (strlen(line) <= 1) continue;
        
        // Parse JSON
        JSONPaper* jsonPaper = parseJSONPaper(line);
        if (jsonPaper == NULL) continue;
        
        printf("📄 Processing: %.50s%s\n", 
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
        if (paperCount % 10 == 0) {
            printf("   📊 Processed %d papers (%d entries)...\n", 
                   paperCount, processedEntries);
        }
        

        if (paperCount >= 1000) {
            printf("⚠️  Limiting to first 100 papers for testing...\n");
            break;
        }
    }
    
    fclose(file);
    
    loader->totalPapers = processedEntries;
    loader->totalFields = countAVLNodes(loader->avlRoot);
    
    printf("\n✅ Berhasil memuat %d entries dari %d papers!\n", 
           processedEntries, paperCount);
    printf("📊 Total bidang studi unik: %d\n\n", loader->totalFields);
    
    return paperCount;
}

// Utility function untuk cleanup memory
void freeJSONPaper(JSONPaper* paper) {
    if (paper != NULL) {
        free(paper);
    }
}
