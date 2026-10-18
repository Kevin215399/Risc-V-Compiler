#ifndef BINARY_TREE
#define BINARY_TREE

#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "../Common.h"
#include "GeneralList.h"

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
TreeNode *TreeGetNodeConstant(TreeNode *root, uint8_t *navMap, uint8_t listLength)
{
    TreeNode *currentNode = root;
    for (int i = 0; i < listLength; i++)
    {
        uint8_t child = navMap[i];
        if (child >= currentNode->children.count)
        {
            printf("navigate fail\n");
            printf("only has %d children\n", currentNode->children.count);
            return NULL;
        }
        currentNode = (TreeNode *)ListGetIndex(&currentNode->children, child);
    }
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
void ClearTree(TreeNode *root)
{
    if (root == NULL)
    {
        return;
    }
    GeneralList exploreMap;
    InitializeList(&exploreMap);
    if (root->children.count == 0)
    {
        if (root->content != NULL)
            free(root->content);
        free(root);
        return;
    }
    TreeNode *currentNode = root;
    PushByte(&exploreMap, currentNode->children.count - 1);
    currentNode = (TreeNode *)PeekList(&currentNode->children);

    while (true)
    {
        printf("Delete tree loop: ");
        for (int i = 0; i < exploreMap.count; i++)
        {
            printf("%d", (int)*((uint8_t *)ListGetIndex(&exploreMap, i)));
        }
        printf("\n");

        if (currentNode->children.count == 0)
        {
        backtrack:
            printf("deleting...\n");
            if (currentNode->content != NULL)
                free(currentNode->content);
            free(currentNode);
            uint8_t *topValue = (uint8_t *)PeekList(&exploreMap);
            printf("Node deleted\n");
            // Go to the next left node
            while ((*topValue) == 0)
            {
                // Go up
                free(PopList(&exploreMap));
                printf("exploring UP\n");

                if (exploreMap.count == 0)
                {
                    // Reached root node
                    if (root->content != NULL)
                        free(root->content);
                    free(root);
                    return;
                }
                topValue = (uint8_t *)PeekList(&exploreMap);
            }

            (*topValue)--;
            currentNode = TreeGetNode(root, &exploreMap);
        }
        else
        {
            PushByte(&exploreMap, currentNode->children.count - 1);
            currentNode = (TreeNode *)PeekList(&currentNode->children);
        }
    }

    printf("tree deleted");
    return;
}

#endif