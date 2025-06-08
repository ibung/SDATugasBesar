#include "avl_paper_loader.h"
#include "../include/cJSON.h"
#include <ctype.h>

// ========== FUNGSI UTILITAS UNTUK AVL TREE ==========

// Mendapatkan tinggi node AVL
int getHeight(AVLNode* node) {
    if (node == NULL) return 0;
    return node->height;
}

// Mendapatkan balance factor node AVL
int getBalance(AVLNode* node) {
    if (node == NULL) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Fungsi untuk melakukan rotasi kanan (Right Rotation)
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    
    // Lakukan rotasi
    x->right = y;
    y->left = T2;
    
    // Update tinggi
    y->height = 1 + ((getHeight(y->left) > getHeight(y->right)) ? getHeight(y->left) : getHeight(y->right));
    x->height = 1 + ((getHeight(x->left) > getHeight(x->right)) ? getHeight(x->left) : getHeight(x->right));
    
    return x; // Node x menjadi root baru
}

// Fungsi untuk melakukan rotasi kiri (Left Rotation)
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    
    // Lakukan rotasi
    y->left = x;
    x->right = T2;
    
    // Update tinggi
    x->height = 1 + ((getHeight(x->left) > getHeight(x->right)) ? 
                     getHeight(x->left) : getHeight(x->right));
    y->height = 1 + ((getHeight(y->left) > getHeight(y->right)) ? 
                     getHeight(y->left) : getHeight(y->right));
    
    return y; // Node y menjadi root baru
}

// ========== FUNGSI UNTUK PAPER LOADER ==========

// Inisialisasi Paper Loader
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

// ========== FUNGSI UNTUK AVL NODE ==========

// Membuat node AVL baru untuk bidang studi
AVLNode* createAVLNode(const char* fieldOfStudy) {
    AVLNode* newNode = (AVLNode*)malloc(sizeof(AVLNode));
    strcpy(newNode->fieldOfStudy, fieldOfStudy);
    newNode->paperList = NULL;
    newNode->paperCount = 0;
    newNode->height = 1;
    newNode->left = NULL;
    newNode->right = NULL;
    
    return newNode;
}

// Insert paper ke AVL Tree berdasarkan bidang studi
AVLNode* insertToAVL(AVLNode* root, Paper* paper) {
    // 1. Lakukan BST insertion biasa
    if (root == NULL) {
        AVLNode* newNode = createAVLNode(paper->bidang_studi);
        
        // Buat CitationNode untuk paper ini
        CitationNode* citationNode = (CitationNode*)malloc(sizeof(CitationNode));
        citationNode->paper = paper;
        citationNode->next = NULL;
        
        newNode->paperList = citationNode;
        newNode->paperCount = 1;
        
        printf("   Bidang studi baru '%s' ditambahkan ke AVL Tree\n", paper->bidang_studi);
        return newNode;
    }
    
    int cmp = strcmp(paper->bidang_studi, root->fieldOfStudy);
    
    if (cmp < 0) {
        root->left = insertToAVL(root->left, paper);
    }
    else if (cmp > 0) {
        root->right = insertToAVL(root->right, paper);
    }
    else {
        // Bidang studi sudah ada, tambahkan paper ke list
        CitationNode* newCitationNode = (CitationNode*)malloc(sizeof(CitationNode));
        newCitationNode->paper = paper;
        newCitationNode->next = root->paperList;
        root->paperList = newCitationNode;
        root->paperCount++;
        
        printf("   Paper '%s' ditambahkan ke bidang '%s' (total: %d)\n", 
               paper->title, paper->bidang_studi, root->paperCount);
        return root;
    }
    
    // 2. Update tinggi node
    root->height = 1 + ((getHeight(root->left) > getHeight(root->right)) ? 
                        getHeight(root->left) : getHeight(root->right));
    
    // 3. Cek balance factor
    int balance = getBalance(root);
    
    // 4. Jika tidak seimbang, lakukan rotasi
    
    // Left Left Case
    if (balance > 1 && strcmp(paper->bidang_studi, root->left->fieldOfStudy) < 0) {
        printf("  Melakukan rotasi kanan (LL Case)\n");
        return rotateRight(root);
    }
    
    // Right Right Case
    if (balance < -1 && strcmp(paper->bidang_studi, root->right->fieldOfStudy) > 0) {
        printf("  Melakukan rotasi kiri (RR Case)\n");
        return rotateLeft(root);
    }
    
    // Left Right Case
    if (balance > 1 && strcmp(paper->bidang_studi, root->left->fieldOfStudy) > 0) {
        printf("  Melakukan rotasi kiri-kanan (LR Case)\n");
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    
    // Right Left Case
    if (balance < -1 && strcmp(paper->bidang_studi, root->right->fieldOfStudy) < 0) {
        printf("  Melakukan rotasi kanan-kiri (RL Case)\n");
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    
    return root;
}

// Mencari node berdasarkan bidang studi
AVLNode* findFieldInAVL(AVLNode* root, const char* fieldOfStudy) {
    if (root == NULL) return NULL;
    
    int cmp = strcmp(fieldOfStudy, root->fieldOfStudy);
    
    if (cmp == 0) return root;
    else if (cmp < 0) return findFieldInAVL(root->left, fieldOfStudy);
    else return findFieldInAVL(root->right, fieldOfStudy);
}

// ========== FUNGSI UNTUK JSON PARSING ==========

// Fungsi untuk membersihkan whitespace
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


void displayFieldsInOrderAscending(AVLNode* root) {
    if (root != NULL) {
        displayFieldsInOrderAscending(root->left);
        printf("📚 %-25s | Papers: %2d | Height: %d | Balance: %2d\n", 
               root->fieldOfStudy, root->paperCount, root->height, getBalance(root));
        displayFieldsInOrderAscending(root->right);
    }
}

// Menampilkan AVL Tree secara Reverse In-Order (Descending - Z to A)
void displayFieldsInOrderDescending(AVLNode* root) {
    if (root != NULL) {
        displayFieldsInOrderDescending(root->right);
        printf("📚 %-25s | Papers: %2d | Height: %d | Balance: %2d\n", 
               root->fieldOfStudy, root->paperCount, root->height, getBalance(root));
        displayFieldsInOrderDescending(root->left);
    }
}

// Menampilkan struktur AVL Tree dengan indentasi
void displayAVLTree(AVLNode* root, int level) {
    if (root != NULL) {
        displayAVLTree(root->right, level + 1);
        
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        printf("├── %s (P:%d, H:%d, B:%d)\n", 
               root->fieldOfStudy, root->paperCount, root->height, getBalance(root));
        
        displayAVLTree(root->left, level + 1);
    }
}

// Menampilkan semua paper dalam bidang tertentu
void displayPapersByField(PaperLoader* loader, const char* fieldOfStudy) {
    AVLNode* fieldNode = findFieldInAVL(loader->avlRoot, fieldOfStudy);
    
    if (fieldNode == NULL) {
        printf("❌ Bidang studi '%s' tidak ditemukan dalam AVL Tree!\n", fieldOfStudy);
        return;
    }
    
    printf("\n🔍 PAPERS IN FIELD: %s\n", fieldOfStudy);
    printf("══════════════════════════════════════════════════════════════\n");
    printf("📊 Total papers: %d\n\n", fieldNode->paperCount);
    
    CitationNode* current = fieldNode->paperList;
    int index = 1;
    
    while (current != NULL) {
        Paper* paper = current->paper;
        printf("📄 Paper #%d:\n", index);
        printf("   📋 Judul: %.60s%s\n", paper->title, 
               strlen(paper->title) > 60 ? "..." : "");
        printf("   👤 Author: %s\n", paper->author);
        printf("   📅 Tahun: %d\n", paper->year);
        printf("   📊 Citations: %d\n", paper->citations);
        printf("   🏷️  Bidang: %s\n", paper->bidang_studi);
        printf("   ────────────────────────────────────────\n");
        
        current = current->next;
        index++;
    }
}



// ========== FUNGSI HELPER ==========

// Menghitung jumlah node dalam AVL Tree
int countAVLNodes(AVLNode* root) {
    if (root == NULL) return 0;
    return 1 + countAVLNodes(root->left) + countAVLNodes(root->right);
}

// ========== FUNGSI CLEANUP ==========

// Membersihkan AVL Tree
void freeAVL(AVLNode* root) {
    if (root != NULL) {
        freeAVL(root->left);
        freeAVL(root->right);
        
        // Free citation nodes dalam field ini
        CitationNode* current = root->paperList;
        while (current != NULL) {
            CitationNode* temp = current;
            current = current->next;
            // Note: Paper* akan di-free oleh CitationManager
            free(temp);
        }
        
        free(root);
    }
}

// Membersihkan Paper Loader
void freePaperLoader(PaperLoader* loader) {
    if (loader != NULL) {
        freeAVL(loader->avlRoot);
        freeCitationManager(loader->citationManager);
        free(loader);
        printf("Paper Loader berhasil dibersihkan dari memory!\n");
    }
}