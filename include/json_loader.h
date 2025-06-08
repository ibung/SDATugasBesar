#ifndef JSON_LOADER_H
#define JSON_LOADER_H

#include "avl_paper_loader.h" // Menggunakan struktur AVLNode
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


int loadJSONData(PaperLoader* loader, const char* filename);
JSONPaper* parseJSONPaper(const char* jsonLine);
Paper* convertJSONToPaper(JSONPaper* jsonPaper, const char* specificField);
PaperLoader* initPaperLoader();
void freePaperLoader(PaperLoader* loader);
// char* extractJSONString(const char* json, const char* key);
// int extractJSONInt(const char* json, const char* key);
// int extractFieldsOfStudy(const char* json, char fields[][150]);
// void extractAuthorsFromJSON(const char* json, char* result);
void trimWhitespace(char* str);
#endif // JSON_LOADER_H