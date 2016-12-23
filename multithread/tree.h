#ifndef TREE_HEADER
#define TREE_HEADER

#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>




typedef struct Tree_node{
	int node_value;
	unsigned int child_num;
	struct Tree_node ** child;
}tree_node;


typedef tree_node Tree;

typedef void (*DatafreeFunc)(const void *);

Tree* init_tree (void);
Tree* createNode(int children_num,int node_value);
void freeTree (Tree * tree, DatafreeFunc dFree );
int appendChild(tree_node * root,int data);
void deleteChild(Tree * root, int index, DatafreeFunc dFree);
int insertChild(tree_node * root,int index, int new_node_data);
Tree* createTree(void);
Tree* insertchild(tree_node * node, int new_value);
Tree* createTree_num(int val);
//int insert_child(tree_node * root,int value);


#endif/*end LIST_HEADER*/
