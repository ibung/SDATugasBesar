#ifndef JSON_LOADER_H
#define JSON_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "citation_stack.h"
#include "cJSON.h"

// Forward declaration
typedef struct AVLNode AVLNode;

// Struktur untuk menyimpan data JSON yang diparsing
typedef struct JSONPaper {
    char id[100];
    char title[300];
    char authors[100];  // Nama author pertama saja
    int year;
    char fieldsOfStudy[5][150];  // Max 5 fields
    int fieldCount;
    int citationCount;  // Dari array inCitations
} JSONPaper;

// Struktur utama Paper Loader
typedef struct PaperLoader {
    AVLNode* avlRoot;
    CitationManager* citationManager;
    int totalPapers;
    int totalFields;
} PaperLoader;

// Function declarations
PaperLoader* initPaperLoader();
int loadJSONData(PaperLoader* loader, const char* filename);
JSONPaper* parseJSONPaper(cJSON* jsonObject);
void extractFirstAuthor(cJSON* authorsArray, char* result);
int extractFieldsOfStudy(cJSON* fieldsArray, char fields[][150]);
void trimWhitespace(char* str);
char* getJSONString(cJSON* json, const char* key, const char* defaultValue);
int getJSONInt(cJSON* json, const char* key, int defaultValue);
void freeJSONPaper(JSONPaper* paper);

#endif