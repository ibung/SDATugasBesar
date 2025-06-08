#ifndef AVL_PAPER_LOADER_H
#define AVL_PAPER_LOADER_H

#include "citation_stack.h"  // Menggunakan struktur yang sudah ada
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ========== STRUKTUR TAMBAHAN UNTUK AVL TREE ==========

// Node AVL Tree berdasarkan bidang studi
typedef struct AVLNode {
    char fieldOfStudy[150];         // Bidang studi (sesuai dengan Paper->bidang_studi)
    CitationNode* paperList;        // Linked list citation dalam bidang ini
    int paperCount;                 // Jumlah paper dalam bidang ini
    int height;                     // Tinggi node untuk balancing AVL
    struct AVLNode* left;           // Pointer ke anak kiri
    struct AVLNode* right;          // Pointer ke anak kanan
} AVLNode;

// ========== STRUKTUR DATA UNTUK PAPER LOADER ==========
// Manager untuk Data Loader & AVL Builder (Anggota 1)
typedef struct PaperLoader {
    AVLNode* avlRoot;               // Root dari AVL Tree berdasarkan bidang studi
    CitationManager* citationManager; // Manager citation yang sudah ada
    int totalPapers;                // Total paper yang dimuat dari JSON
    int totalFields;                // Total bidang studi unik
} PaperLoader;

// ========== STRUKTUR UNTUK JSON PARSING ==========
// Struktur sementara untuk parsing JSON
typedef struct JSONPaper {
    char id[100];                   // ID dari JSON
    char title[300];                // Title dari JSON (increased size)
    char authors[200];              // Authors (gabungan jika multiple)
    char fieldsOfStudy[5][150];     // Array bidang studi (bisa multiple)
    int fieldCount;                 // Jumlah bidang studi
    int year;                       // Tahun publikasi
    char inCitations[50][100];      // Array citation IDs
    int citationCount;              // Jumlah citations
} JSONPaper;

// ========== DEKLARASI FUNGSI UTAMA ANGGOTA 1 ==========

// === Fungsi untuk Paper Loader ===
PaperLoader* initPaperLoader();
void freePaperLoader(PaperLoader* loader);

// === Fungsi untuk AVL Tree Operations ===
AVLNode* createAVLNode(const char* fieldOfStudy);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertToAVL(AVLNode* root, Paper* paper);
AVLNode* findFieldInAVL(AVLNode* root, const char* fieldOfStudy);
void displayAVLTree(AVLNode* root, int level);
void displayFieldsInOrder(AVLNode* root);
void freeAVL(AVLNode* root);

// === Fungsi untuk JSON Data Loading ===
int loadJSONData(PaperLoader* loader, const char* filename);
JSONPaper* parseJSONPaper(const char* jsonLine);
Paper* convertJSONToPaper(JSONPaper* jsonPaper, const char* specificField);
void addCitationsToPaper(Paper* paper, JSONPaper* jsonPaper);

// === Fungsi JSON Parsing Utilities ===
char* extractJSONString(const char* json, const char* key);
int extractJSONInt(const char* json, const char* key);
int extractFieldsOfStudy(const char* json, char fields[][150]);
void extractAuthorsFromJSON(const char* json, char* result);

// === Fungsi untuk Display dan Statistics ===
void displayPapersByField(PaperLoader* loader, const char* fieldOfStudy);
void displayAllFieldsStatistics(PaperLoader* loader);
void displayLoaderStatistics(PaperLoader* loader);

// === Fungsi Helper ===
int countAVLNodes(AVLNode* root);
void trimWhitespace(char* str);

// === Fungsi untuk Demo dan Testing ===
void demonstrateAVLOperations(PaperLoader* loader);
void loadSampleData(PaperLoader* loader);

#endif