#ifndef DZAKIR_BST_H
#define DZAKIR_BST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Paper Paper;
typedef struct BST_Node {
    char field[100];
    Paper* papers_head;
    struct BST_Node* left;
    struct BST_Node* right;
} BST_Node;

BST_Node* create_bst_node(const char* field);
BST_Node* insert_field(BST_Node* root, const char* field);
BST_Node* search_field(BST_Node* root, const char* field);
void inorder_traversal(BST_Node* root);
void free_bst(BST_Node* root);

#endif