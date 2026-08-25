#ifndef BINARY_TREE
#define BINARY_TREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode
{
    struct TreeNode* parent;
    struct TreeNode* left;
    struct TreeNode* right;
    void* content;
} TreeNode;



#endif