#ifndef BINARY_TREE
#define BINARY_TREE

#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "../Common.h"

int min(int a, int b);

typedef struct TreeNode
{
    struct TreeNode *parent;
    GeneralList children;
    void *content;
} TreeNode;

void InitializeTree(TreeNode *root)
{
    root->parent = NULL;
    root->content = NULL;
    InitializeList(&root->children);
}

TreeNode *TreeGetNode(TreeNode *root, GeneralList *navMap)
{
    TreeNode *currentNode = root;
    for (int i = 0; i < navMap->count; i++)
    {
        uint8_t *child = (uint8_t *)ListGetIndex(navMap, i);
        // printf("navigate: %d\n", (int)(*child));
        if ((*child) >= currentNode->children.count)
        {
            printf("navigate fail\n");
            printf("only has %d children\n", currentNode->children.count);
            return NULL;
        }
        currentNode = (TreeNode *)ListGetIndex(&currentNode->children, (*child));
    }
    // printf("navigate sucess\n");
    return currentNode;
}
TreeNode *TreeGetNodeLimited(TreeNode *root, GeneralList *navMap, uint8_t maxInstructions)
{
    TreeNode *currentNode = root;
    int iterations = min(navMap->count, maxInstructions);
    for (int i = 0; i < iterations; i++)
    {
        uint8_t *child = (uint8_t *)ListGetIndex(navMap, i);
        // printf("navigate: %d\n", (int)(*child));
        if ((*child) >= currentNode->children.count)
        {
            printf("navigate fail\n");
            printf("only has %d children\n", currentNode->children.count);
            return NULL;
        }
        currentNode = (TreeNode *)ListGetIndex(&currentNode->children, (*child));
    }
    // printf("navigate sucess\n");
    return currentNode;
}
void AppendTree(TreeNode *parent, void *content)
{
    printf("pushing tree\n");
    TreeNode *newNode = malloc(sizeof(TreeNode));
    InitializeTree(newNode);
    newNode->content = content;
    newNode->parent = parent;

    PushList(&parent->children, newNode);
}
bool AppendTreeFromRoot(TreeNode *root, GeneralList *parentNavMap, void *content)
{
    TreeNode *parent = TreeGetNode(root, parentNavMap);
    if (parent == NULL)
        return false;
    AppendTree(parent, content);
    return true;
}

#endif