#include "tree.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTDEBUG(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#define PRINTDEBUG(...)
#endif

#ifndef NULL
#define NULL 0
#endif

#ifdef LIST_CONFSIZE
#define LIST_SIZE LIST_CONFSIZE
#else
#define LIST_SIZE 1
#endif


Tree * solvTree;

/*-----------------------MACROS-------------------------------*/
#define NODE_MEM (tree_node*) malloc(1 * sizeof(tree_node))
#define MEM_REALLOC(size) (tree_node**)realloc(tree_node,(size)* sizeof(tree_node*))


Tree* createTree(void){
	tree_node * node = NODE_MEM;
	node->node_value=-1;
	node->child_num=0;
	node->child=NULL;
	return node;
}


Tree* insertchild(tree_node * node, int new_value){
	tree_node * temp_node = NODE_MEM;
	node->child_num+=1;
	temp_node->node_value=new_value;
	temp_node->child_num=0;
	temp_node->child=NULL;
	node->child = (tree_node**)realloc(node->child,(node->child_num)* sizeof(tree_node*));
	node->child[node->child_num-1]=temp_node;
	return node;
}

Tree* createNode(int children_num,int node_value){
	/*allocate space for a new children memory */
	tree_node * node = (tree_node*) malloc(1 * sizeof(tree_node));
	node->child_num=children_num;
	node->node_value=node_value;
	node->child = (tree_node **) malloc (children_num * sizeof(tree_node));
	return node;
}

void freeTree (Tree * tree, DatafreeFunc dFree ){
	unsigned int child_num;
	if(tree==NULL){
		PRINTF("the three is null we cannot execute the free_tree operation\n");
		return;
	}

	/*free the children recursively*/
	int i;
	for (i=0; i<tree->child_num;i++){
		freeTree(tree->child[i],dFree);
	}
	/*free the child array*/
	free(tree->child);
	free(tree);
}



int appendChild(tree_node * root,int data){
	root->child_num++;
	/*Reallocate the children array */
	root->child= (tree_node**)realloc(root->child,(root->child_num)*sizeof(tree_node*));
	/*add the newNode into the child array and increment the degree*/
	root->child[root->child_num-1]=createNode(0,data);
	/*return the index of the new child*/
	return root->child_num-1;
}



void deleteChild(Tree * root, int index, DatafreeFunc dFree){
	unsigned int i;
	/* delete the child*/
	freeTree(root->child[index],dFree);
	/*remove the defunct child*/
	for (i=index;i<root->child_num-1; i++){
		root->child[i]=root->child[i-1];
	}
	/*Reallocate */
	root->child_num--;
	root->child=(tree_node**)realloc(root->child,(root->child_num)*sizeof(tree_node*));
}





