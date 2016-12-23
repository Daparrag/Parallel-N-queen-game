#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "tree.h"
#include "queen.h"

#define DEBUG 1
#define THREADS 0
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


//Tree * solv_tree;
//struct queen * partial_seq;
//int solution_id;
//int _index;
//int nsolv_ix;

struct variables{	
Tree * solv_tree;
struct queen * partial_seq;
int solution_id;
int _index;
int nsolv_ix;
};


struct variables global_variables;

#if THREADS
int n_threads;
int f_threads;
/*prototype for lock_unlock*/
static void lock(void);
static void unlock(void);
//static void thread_management (tree_node *root);

static char free_threads(void){
	return f_threads;
}

static int new_threads_allocation(void){
	
}

//static void thread_management (tree_node *root){
//	
//}

#endif



static void remove_last_queen(void);
static queen * head_partial_seq(void);
static queen * get_next_queen(struct queen * ptrQueen);
static void init_queenSeq(void);
static void print_sequences(void);

static queen * head_partial_seq(void){
	return (global_variables.partial_seq);
}

static queen * get_next_queen(struct queen * ptrQueen){
	return ptrQueen->next_queen;
}


static void init_queenSeq(void){
	global_variables.partial_seq = (struct queen*)malloc(sizeof(struct queen));
	global_variables.partial_seq->next_queen=NULL;
	global_variables.partial_seq->tail=NULL;
}


static void print_sequences(void){
	struct queen *ptr=head_partial_seq();
	if(ptr==NULL){
		//printf("print seq: sequence is empty\n");
		return;
	}
	
	while(ptr!=NULL){
		printf("print seq: row %u, column %u \n",ptr->row,ptr->column);
		ptr=ptr->next_queen;
	}
}


void init_solv(void){
	init_qarray();
	global_variables.solv_tree=createTree();
	init_queenSeq();
	global_variables.solution_id=0;
	global_variables._index=0;
	global_variables.nsolv_ix=0;
}


char safe_two_queens(struct queen* queen_prev, struct queen* queen_next){
	if((queen_prev->row != queen_next->row)&&(
		abs((queen_prev->column)-(queen_next->column))
		!= abs((queen_prev->row) - (queen_next->row)))
		) return 1;
	return 0;
};



char safe_sequence(struct queen* queen, struct queen* next_queen){
	struct queen * ptr_queen;
	
	//printf("next_queen to test row: %u , column: %u\n",next_queen->row,next_queen->column);

	for(ptr_queen=head_partial_seq();ptr_queen!=NULL;ptr_queen=get_next_queen(ptr_queen)){
		//printf("local queen to test row: %u , column: %u\n",ptr_queen->row,ptr_queen->column);
		if(safe_two_queens(ptr_queen,next_queen)==0){
			return 0;
		}
	}

	return 1;
}


char update_sequence(struct queen* new_queen){
	new_queen->next_queen=NULL;
	if(global_variables.partial_seq==NULL)init_queenSeq();
	if(global_variables.partial_seq->next_queen==NULL && global_variables.partial_seq->tail==NULL){
		global_variables.partial_seq=new_queen;
		global_variables.partial_seq->tail=new_queen;
		//return 1;
	}else{
		global_variables.partial_seq->tail->next_queen=new_queen;
		global_variables.partial_seq->tail=new_queen;
		
	}
	return 1;
}



static void remove_last_queen(void){
	struct queen * ptr_queen=global_variables.partial_seq;
	struct queen * ptr_last;
	
	if(ptr_queen==NULL){
		//printf("list is already empty\n");
		return;
	}else if(ptr_queen->next_queen==NULL){
		//printf("clearning the list_queen \n");
		global_variables.partial_seq=NULL;
		free(ptr_queen);
		return;
	}
	
	while(ptr_queen->next_queen!=NULL){
		ptr_last=ptr_queen;
		ptr_queen=ptr_queen->next_queen;
	}
	ptr_last->next_queen=NULL;
	global_variables.partial_seq->tail=ptr_last;
	free(ptr_queen);
	
	
}

tree_node * update_child(tree_node * root, int val){
	insertchild(root,val);
	//_index++;
	return root;
}



int Update_solution(struct  queen * queen,int nsolutions){
#if THREADS
lock();
#endif	
	init_list(nsolutions);
	while(queen!=NULL){
		insert_element_to_list(nsolutions,queen->row,queen->column);
		queen= queen->next_queen;
	}
	
	remove_last_queen();
	global_variables.nsolv_ix+=1;
#if THREADS
unlock();
#endif		
	return 1;
	
}




int funct_solution_seq(tree_node *root,int _level, int Nqueens){
	//printf("start1\n");
	struct queen * new_queen = (struct queen*)malloc(sizeof(struct queen));
	struct queen temp_queen;
	int new_solv=0;
	unsigned int i;
	if(root->node_value==-1){
		new_solv=1;
		for (i=0;i<Nqueens;i++){
			root = update_child(root,i);
		}	
	}else{
		new_queen->row=root->node_value;
		new_queen->column=_level;
		update_sequence(new_queen);
		if(_level < Nqueens-1){
			_level+=1;
			temp_queen.column =_level;
			for (i=0; i< Nqueens; i++){
				temp_queen.row=i;
				if(/*critical seccion start lock()*/
					safe_sequence(global_variables.partial_seq,&temp_queen)==1
					/*critical seccion end unlocklock()*/)
					{
					new_solv=1;
					update_child(root,i);
				}
			}
		}else{
			/*the algorithm find a solution*/
			/*critical seccion satart lock()*/
			Update_solution(global_variables.partial_seq, global_variables.nsolv_ix);
			/*crtical seccion start unlock()*/
			_level-=1;
			return 1;
		}
	}
	if(new_solv){
		
		for(i=0;i < root->child_num; i++){
			funct_solution_seq(root->child[i],_level,Nqueens);
		}
		remove_last_queen();
		return 1;
		
	}else if(_level != Nqueens){
		remove_last_queen();
		_level-=1;
		return 0;
	}
	
return 1;	
}

/* Print the results at the end of the run */
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


int main(int argc, char *argv[]){

	assert(argc>1);
	int queen_number = atoi(argv[1]);
	
	if(queen_number<=3){
	  PRINTF("Impossible to find a solution for (N=%u)\n",queen_number);
	  return 0;
	}


/**************************************************************************************************************************************************************************************/
PRINTF("Starting the computation of Nqueens with (N=%u)\n",queen_number);	
init_solv();
time_t t1, t2;

time(&t1);
funct_solution_seq(global_variables.solv_tree,0,queen_number);
time(&t2);
int nSolutions=get_queen_array_size();//nsolv_ix;
printf("num of solutions: %u\n",nSolutions);
printResults(&t1, &t2);
/*
int i;
for (i=0;i<nSolutions;i++){
	print_queen_list(i);
}*/
return 1;
}
