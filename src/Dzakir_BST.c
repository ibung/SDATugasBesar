#include "include/Dzakir_BST.h"

BST_Node* create_bst_node(const char* field) {
    BST_Node* node = (BST_Node*)malloc(sizeof(BST_Node));
    strcpy(node->field, field);
    node->papers_head = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

BST_Node* insert_field(BST_Node* root, const char* field) {
    if (root == NULL) {
        return create_bst_node(field);
    }
    
    int cmp = strcmp(field, root->field);
    if (cmp < 0) {
        root->left = insert_field(root->left, field);
    } else if (cmp > 0) {
        root->right = insert_field(root->right, field);
    }
    
    return root;
}

BST_Node* search_field(BST_Node* root, const char* field) {
    if (root == NULL || strcmp(field, root->field) == 0) {
        return root;
    }
    
    if (strcmp(field, root->field) < 0) {
        return search_field(root->left, field);
    }
    
    return search_field(root->right, field);
}
