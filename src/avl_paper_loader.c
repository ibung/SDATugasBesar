#include "../include/avl_paper_loader.h"
#include "../include/cJSON.h"
#include <ctype.h>
#include "../include/json_loader.h"

// Mendapatkan tinggi dari node AVL
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
    if (root == NULL) {
        AVLNode* newNode = createAVLNode(paper->field_of_study);
        
        CitationNode* citationNode = (CitationNode*)malloc(sizeof(CitationNode));
        citationNode->paper = paper;
        citationNode->next = NULL;
        
        newNode->paperList = citationNode;
        newNode->paperCount = 1;
        
        printf("   Bidang studi baru '%s' ditambahkan ke AVL Tree\n", paper->field_of_study);
        return newNode;
    }
    
    int cmp = strcmp(paper->field_of_study, root->fieldOfStudy);
    
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
               paper->title, paper->field_of_study, root->paperCount);
        return root;
    }
    
    // 2. Update tinggi node
    root->height = 1 + ((getHeight(root->left) > getHeight(root->right)) ? getHeight(root->left) : getHeight(root->right));
    
    // 3. Cek balance factor
    int balance = getBalance(root);
    
    // 4. Jika tidak seimbang, lakukan rotasi
    
    // Left Left Case
    if (balance > 1 && strcmp(paper->field_of_study, root->left->fieldOfStudy) < 0) {
        printf("  Melakukan rotasi kanan (LL Case)\n");
        return rotateRight(root);
    }
    
    // Right Right Case
    if (balance < -1 && strcmp(paper->field_of_study, root->right->fieldOfStudy) > 0) {
        printf("  Melakukan rotasi kiri (RR Case)\n");
        return rotateLeft(root);
    }
    
    // Left Right Case
    if (balance > 1 && strcmp(paper->field_of_study, root->left->fieldOfStudy) > 0) {
        printf("  Melakukan rotasi kiri-kanan (LR Case)\n");
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    
    // Right Left Case
    if (balance < -1 && strcmp(paper->field_of_study, root->right->fieldOfStudy) < 0) {
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



// Fungsi untuk membersihkan whitespace
void displayFieldsInOrderAscending(AVLNode* root) {
    if (root != NULL) {
        displayFieldsInOrderAscending(root->left);
        printf("%-25s | Papers: %2d | Height: %d | Balance: %2d\n", 
               root->fieldOfStudy, root->paperCount, root->height, getBalance(root));
        displayFieldsInOrderAscending(root->right);
    }
}

// Menampilkan AVL Tree secara Reverse In-Order (Descending - Z to A)
void displayFieldsInOrderDescending(AVLNode* root) {
    if (root != NULL) {
        displayFieldsInOrderDescending(root->right);
        printf("%-25s | Papers: %2d | Height: %d | Balance: %2d\n", 
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
    
    printf("\nPAPERS IN FIELD: %s\n", fieldOfStudy);
    printf("══════════════════════════════════════════════════════════════\n");
    printf("Total papers: %d\n\n", fieldNode->paperCount);
    
    CitationNode* current = fieldNode->paperList;
    int index = 1;
    
    while (current != NULL) {
        Paper* paper = current->paper;
        printf("Paper #%d:\n", index);
        printf("   udul: %.60s%s\n", paper->title, 
               strlen(paper->title) > 60 ? "..." : "");
        printf("  Author: %s\n", paper->authors);
        printf("  Tahun: %d\n", paper->year);
        printf("  Citations: %d\n", paper->citation_count);
        printf("  Bidang: %s\n", paper->field_of_study);
        printf("   ────────────────────────────────────────\n");
        
        current = current->next;
        index++;
    }
}


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