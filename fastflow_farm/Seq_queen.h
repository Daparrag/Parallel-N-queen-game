/*
 * This is the Queen class used in all the version
 * of the n-queen algorithm
 */

/* 
 * File:   queen.h
 * Author: Diego Alejandro Parra
 *
 * 
 */

#ifndef _QUEEN_
#define _QUEEN_

#include <vector>
using namespace std;

#include "tree.h"
#include "queen.h"


#define DEBUG 0
#define THREADS 0
#define QUEUE_SIZE 322
#define STORE_RESULT 0
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








 struct variables{	
Tree * solv_tree;
int nsolv_ix;
};


struct passing_variables{
	tree_node *in_root;
	int in_level;
	int in_Nqueens;
	struct queen * partial_seq;
};

struct variables global_variables;
struct passing_variables;
/*=======================================================STATIC FUNCTION PROTOTYPE==========================================================================*/ 
static void clear_queen_list(struct queen **list_queen);
static queen * get_next_queen(struct queen * ptrQueen);
static void print_sequences(struct queen * queen_list);
static struct queen * Clone_queen_list(struct queen * original_queen_list);
static void printResults(time_t* pt1, time_t* pt2);
static char lastPieceIsValid(int n, vector<int> &rows);
/*==========================================================================================================================================================*/ 


static queen * get_next_queen(struct queen * ptrQueen){
	return ptrQueen->next_queen;
}


static void print_sequences(struct queen * queen_list){
	struct queen *ptr=queen_list;
	if(ptr==NULL){
		return;
	}
	
	while(ptr!=NULL){
		printf("print seq: row %u, column %u \n",ptr->row,ptr->column);
		ptr=ptr->next_queen;
	}
}


static struct queen * Clone_queen_list(struct queen * original_queen_list){
	if(original_queen_list==NULL) return NULL;
	struct queen * copy_queen_list = (struct queen*)malloc(sizeof(struct queen));
	if(copy_queen_list==NULL){
		PRINTF("ERROR MALLOC");
	}
	
	copy_queen_list->row = original_queen_list->row;
	copy_queen_list->column = original_queen_list->column;
	copy_queen_list->next_queen=Clone_queen_list(original_queen_list->next_queen);
	struct queen * copy_queen_head=copy_queen_list;
	
	if(copy_queen_list->next_queen==NULL){
		copy_queen_list->tail=NULL;
	}else{
		while(copy_queen_list->next_queen!=NULL){
			copy_queen_list=copy_queen_list->next_queen;
		}
	}
	copy_queen_head->tail=copy_queen_list;
	return copy_queen_head;
	
}



static void clear_queen_list(struct queen **list_queen){
	struct queen *to_delete;
	while((to_delete = *list_queen)){
		 *list_queen = to_delete->next_queen;
		 free(to_delete);
	}
}

static void printResults(time_t* pt1, time_t* pt2)
{
    double secs;
    int hours , mins, intsecs;

    printf("End: \t%s", ctime(pt2));
    secs = difftime(*pt2, *pt1);
    intsecs = (double)secs;
	printf("%d \n",intsecs);
    printf("Calculations took %d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));


    /* Print hours, minutes, seconds */
    hours = intsecs/3600;
    intsecs -= hours * 3600;
    mins = intsecs/60;
    intsecs -= mins * 60;
    if (hours > 0 || mins > 0)
    {
        printf("Equals ");
        if (hours > 0)
        {
            printf("%d hour%s, ", hours, (hours == 1) ? "" : "s");
        }
        if (mins > 0)
        {
            printf("%d minute%s and ", mins, (mins == 1) ? "" : "s");
        }
        printf("%d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));

    }
}





static char lastPieceIsValid(int n, vector<int> &rows)
{
    int row = rows.back();
    int col = rows.size() - 1;

    // Check if last queen is beyond the chess board
    if (row >= n)
		return 0;

    // Otherwise, check position validity against the other queens
    for(size_t i = 0; i < rows.size() - 1; i++)
    {
        // Check if on same row
        if (row == rows[i])
			return 0;

        // check diagonals
        if (abs(row - rows[i]) == abs(col - int(i)))
			return 0;
    }
    return 1;
}








/*=======================================================FUNCTION PROTOTYPE==========================================================================*/ 
void init_solv(void);
char safe_two_queens(struct queen* queen_prev, struct queen* queen_next);
char safe_sequence(struct queen* queen, struct queen* next_queen);
int funct_solution_seq(tree_node * in_root, struct queen * partial_seq,int in_level, int in_Nqueens, int * solution);
tree_node * update_child(tree_node * root, int val);
int Update_solution(struct  queen * queen);
void funct_solution_nonrecursive(int n_queens, int * solutions, vector<int> & rows);
/*===================================================================================================================================================*/ 
 
 
 
 void init_solv(void){
	init_qarray();
	global_variables.solv_tree=createTree();
	global_variables.nsolv_ix=0;
 }
 
 
 char safe_two_queens(struct queen* queen_prev, struct queen* queen_next){
	if((queen_prev->row != queen_next->row)&&(
		abs((queen_prev->column)-(queen_next->column))
		!= abs((queen_prev->row) - (queen_next->row)))
		) return 1;
	return 0;
}



char safe_sequence(struct queen* queen, struct queen* next_queen){
	struct queen * ptr_queen;	
	for (ptr_queen=queen; ptr_queen!=NULL; ptr_queen=get_next_queen(ptr_queen)){
		if(safe_two_queens(ptr_queen,next_queen)==0){
			return 0;
		}
	}
	return 1;
}


struct queen *  update_sequence(struct queen * queen_list, struct queen* new_queen){
	
	if(queen_list==NULL){
		queen_list=(struct queen*) malloc (sizeof(struct queen));
		queen_list->row=new_queen->row;
		queen_list->column=new_queen->column;
		queen_list->next_queen=NULL;
		queen_list->tail=NULL;
	}else{
		struct queen * next_queen = (struct queen*)malloc(sizeof(struct queen));
		next_queen->row=new_queen->row;
		next_queen->column=new_queen->column;
		next_queen->next_queen=NULL;
		if(queen_list->tail==NULL){
			queen_list->tail=next_queen;
			queen_list->next_queen=next_queen;
		}else{
		queen_list->tail->next_queen=next_queen;
		queen_list->tail=next_queen;
		}
	} 
	return queen_list;
}




tree_node * update_child(tree_node * root, int val){
	insertchild(root,val);
	return root;
}


int Update_solution(struct  queen * queen){
#if STORE_RESULT
	int nsolutions = global_variables.nsolv_ix;
	init_list(nsolutions);
	while(queen!=NULL){
		insert_element_to_list(nsolutions,queen->row,queen->column);
		queen= queen->next_queen;
	}
#endif
	global_variables.nsolv_ix++;
	return 1;
}


int funct_solution_seq(tree_node * in_root, struct queen * partial_seq,int in_level, int in_Nqueens, int * solution){
	/*cast the input arguments*/
	//struct passing_variables variables = *(struct passing_variables *)in_variables;
	//tree_node * root=(tree_node*) malloc(1 * sizeof(tree_node));
	tree_node * root = in_root;
	struct queen* local_queen_seq = Clone_queen_list(partial_seq);
	long _level=in_level;
	long _Nqueens=in_Nqueens;
	/*once we get a local copy of the partial list sequence we delete the original one in order to save memory*/
	//clear_queen_list(&variables.partial_seq);
	/*initialization of some control varaibles*/
	int new_solv=0;
	unsigned int i;
	/*now locally is create a new queen which will be the possible solution and entry in the list*/
	//struct queen * new_queen = (struct queen*)malloc(sizeof(struct queen));
	struct queen temp_queen;
	/*evaluate if we are in the root element of the tree*/
	if(root->node_value==-1){
		new_solv=1;
		for (i=0;i<_Nqueens;i++){
			root = update_child(root,i);
			
		}	
	}else{
		/*if we are not in the root of the tree we evaluate all the possible queen solutions vs the current list Copy */
		/*update the input_queen queen_list*/
		temp_queen.row=root->node_value;
		temp_queen.column=_level;
		local_queen_seq = update_sequence(local_queen_seq,&temp_queen);	
		if(_level < _Nqueens-1){
			_level+=1;
			temp_queen.column =_level;
			for (i=0; i< _Nqueens; i++){
				temp_queen.row=i;
				if(/*critical seccion start lock()*/
					safe_sequence(local_queen_seq,&temp_queen)==1
					/*critical seccion end unlocklock()*/)
					{
					new_solv=1;
					update_child(root,i);
				}
			}
		}else{
			/*the algorithm find a solution*/
			/*critical seccion satart lock()*/
			Update_solution(local_queen_seq);
			/*clear the solution list*/
			clear_queen_list(&local_queen_seq);
			/*crtical seccion start unlock()*/
			*(int*)solution +=1;
			//return global_variables.nsolv_ix;
			return 1;
		}
	}
	if(new_solv){
		/*it means that there is at least one new queen to evaluate..*/
		struct passing_variables variables_out;
		variables_out.in_level=_level;
		variables_out.in_Nqueens=_Nqueens;
		variables_out.partial_seq=local_queen_seq;
		for(i=0;i < root->child_num; i++){
			/*create a copy of the list*/
			variables_out.in_root=root->child[i];
			funct_solution_seq(variables_out.in_root,variables_out.partial_seq,variables_out.in_level,variables_out.in_Nqueens,solution);
			
		}
			/*once we send the copy for all childeren we can delete our local copy for safe memory*/
		clear_queen_list(&local_queen_seq);
		return 0;
	}else if(_level != _Nqueens){
		/*discart solution*/
		/*not solution was found then clear the list*/
		clear_queen_list(&local_queen_seq);
		return 0;
	}
	
return 1;	
}


void funct_solution_nonrecursive(int n_queens, int * solutions, vector<int> & rows){


	char done = 0;
	  unsigned long int solu = 0;
	  int row;


	if(rows.size()==0)// Place First Piece
    rows.push_back(0);
	 //vector<int> rows;
	 //int solv=0;

// Loop until a all solutions are found
	while (!done)
    {
 // See if last queen is ok where it is
		if (!lastPieceIsValid(n_queens, rows))
        {
			// Last position is bad, find last valid piece
            row = rows.back();  // Record last row tried
            // If it's out of bounds, backtrack to last vaild piece
			if (row >= n_queens){
				 do{
					 // make sure we haven't found all solutions already
					 if (rows.size() > 1){
						  rows.pop_back();        // Remove invalid piece
						  row = rows.back();      // Record row of last good piece
						  rows.pop_back();        // Remove that piece...
						  rows.push_back(row + 1);    // And now try the next row.
                          row = rows.back();      // See if that row is in bounds
					 }else{
						done = 1;
                        break;  // exit loop
					 }

				 }while (rows.back() >= n_queens);
			}else{// It was within bounds but still bad.  Try the next row.
				rows.pop_back();
                rows.push_back(row + 1);
			}
		}else{
			if (rows.size() == n_queens){
				//*(int*)solutions+=1;
				solu++;
				// Continue finding more soltions (if any)
				row = rows.back();
                rows.pop_back();
                rows.push_back(row + 1);
				
				//if (solu % (n_queens * n_queens) == 0)
					//printf("solutions%u\n",solu);
			}
		// If not, proceed to the next column
            else rows.push_back(0);
		}
	}
	*(int*)solutions=solu;

}

#endif /* QUEEN_HPP */