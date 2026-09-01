#ifndef BINARY_TREE
#define BINARY_TREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

typedef struct TreeNode
{
    GeneralList children;
    void *content;
} TreeNode;

void InitializeTree(TreeNode *root)
{
    root->content = NULL;
    InitializeList(&root->children);
}

TreeNode *TreeGetNode(TreeNode *root, GeneralList *navMap)
{
    TreeNode *currentNode = root;
    for (int i = 0; i < navMap->count; i++)
    {
        uint8_t *child = (uint8_t *)ListGetIndex(navMap, i);
        printf("navigate: %d\n", (int)(*child));
        if ((*child) >= currentNode->children.count)
        {
            printf("navigate fail\n");
            printf("only has %d children\n",currentNode->children.count);
            return NULL;
        }
        currentNode = (TreeNode *)ListGetIndex(&currentNode->children, (*child));
    }
    printf("navigate sucess\n");
    return currentNode;
}
bool AppendTree(TreeNode *parent, void *content)
{
    printf("pushing tree\n");
    TreeNode *newNode = malloc(sizeof(TreeNode));
    InitializeTree(newNode);
    newNode->content = content;

    PushList(&parent->children, newNode);
}
bool AppendTreeFromRoot(TreeNode *root, GeneralList *parentNavMap, void *content)
{
    TreeNode *parent = TreeGetNode(root, parentNavMap);
    AppendTree(parent, content);
}

#endif