#ifndef AVL_PAPER_LOADER_H
#define AVL_PAPER_LOADER_H

#include "citation_stack.h"  // Menggunakan struktur yang sudah ada
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Forward declaration untuk menghindari circular dependency
typedef struct PaperLoader PaperLoader;

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

// === Fungsi untuk AVL Tree Operations ===
AVLNode* createAVLNode(const char* fieldOfStudy);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertToAVL(AVLNode* root, Paper* paper);
AVLNode* findFieldInAVL(AVLNode* root, const char* fieldOfStudy);
void displayAVLTree(AVLNode* root, int level);
void displayFieldsInOrderAscending(AVLNode* root);
void displayFieldsInOrderDescending(AVLNode* root);
void freeAVL(AVLNode* root);
void displayPapersByField(PaperLoader* loader, const char* fieldOfStudy);
void freePaperLoader(PaperLoader* loader);
int countAVLNodes(AVLNode* root);

#endif