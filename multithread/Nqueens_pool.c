/*  (from Sloane's On-Line Encyclopedia of Integer Sequences,
 *   Sequence A000170
 *   http://www.research.att.com/cgi-bin/access.cgi/as/njas/sequences/eisA.cgi?Anum=000170
 *   )
 *
 *   Board Size:       Number of Solutions to          Time to calculate
 *   (length of one        N queens problem:              on 800MHz PC
 *    side of N x N                                    (Hours:Mins:Secs)
 *    chessboard)
 *
 *     1                                  1                    n/a
 *     2                                  0                   < 0 seconds
 *     3                                  0                   < 0 seconds
 *     4                                  2                   < 0 seconds
 *     5                                 10                   < 0 seconds
 *     6                                  4                   < 0 seconds
 *     7                                 40                   < 0 seconds
 *     8                                 92                   < 0 seconds
 *     9                                352                   < 0 seconds
 *    10                                724                   < 0 seconds
 *    11                               2680                   < 0 seconds
 *    12                              14200                   < 0 seconds
 *    13                              73712                   < 0 seconds
 *    14                             365596                  00:00:01
 *    15                            2279184                  00:00:04
 *    16                           14772512                  00:00:23
 *    17                           95815104                  00:02:38
 *    18                          666090624                  00:19:26
*/




#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <string.h>
#include <sys/types.h>
#include <atomic>
#include <math.h>

#include "tree.h"
#include "queen.h"
#include "threadpool.h"


#define DEBUG 1
#define THREADS 1
#define QUEUE_SIZE 65535000
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


pthread_mutex_t mutexqueen;
pthread_mutexattr_t Attr;




struct AtomicCounter {
    std::atomic<int> value;
    void increment(){
        ++value;
    }
    void decrement(){
        --value;
    }
    int get(){
        return value.load();
    }
};




struct variables{
Tree * solv_tree;
AtomicCounter nsolv_ix;
int thread_num;
threadpool_t *thpool;
int thread_count;
int queue_size;
int flags;
};


struct passing_variables{
	tree_node *in_root;
	int in_level;
	int in_Nqueens;
	int set_split;
	double maximum_split;
	int deep_level;
	struct queen * partial_seq;
	
};


struct variables global_variables;
static void lock(void);
static void unlock(void);



/*=================================================STATIC FUCTIONS PROTOTYPES========================================*/
static void unlock(void);
static void lock(void);
static void clear_queen_list(struct queen **list_queen);
static queen * get_next_queen(struct queen * ptrQueen);
static void print_sequences(struct queen * queen_list);
static struct queen * Clone_queen_list(struct queen * original_queen_list);
static void set_pool_parameters(int thread_count,int queue_size,int flags);
static void printResults(time_t* pt1, time_t* pt2);
/*====================================================================================================================*/


#ifndef THREADS
#include <pthread.h>

static void lock(void){
	int rc;
	rc = pthread_mutex_lock (&mutexqueen);
	if(rc!= 0){
		PRINTF("ERROR: processing the mutex_lock\n");
	}
}

static void unlock(void){
	int rc;
	rc = pthread_mutex_unlock(&mutexqueen);
	if(rc!= 0){
		PRINTF("ERROR: processing the mutex_unlock\n");
	}
}	
#else
	static void lock(void){}
	static void unlock(void){}
#endif



/* Print the results at the end of the run */
static void printResults(struct timeb* pt1, struct timeb* pt2)
{
    double secs;
  
    //printf("End: \t%s", ctime(pt2->time);
    //
    //
    //
       double secs_t1 = ((double)(pt1->time)+((double)pt1->millitm/1000));
       double sec_t2 = ((double)(pt2->time)+((double)pt2->millitm/1000));
	secs = (sec_t2 - secs_t1); 
   
    printf("Calculations took second: %f  \n", secs);


    /* Print hours, minutes, seconds */
    //hours = intsecs/3600;
    //intsecs -= hours * 3600;
    //mins = intsecs/60;
    //intsecs -= mins * 60;
    //if (hours > 0 || mins > 0)
    //{
    //    printf("Equals ");
    //    if (hours > 0)
    //    {
    //        printf("%d hour%s, ", hours, (hours == 1) ? "" : "s");
    //    }
    //    if (mins > 0)
    //    {
    //        printf("%d minute%s and ", mins, (mins == 1) ? "" : "s");
    //    }
    //    printf("%d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));

    //}
}



static void clear_queen_list(struct queen **list_queen){
	struct queen *to_delete;
	while((to_delete = *list_queen)){
		 *list_queen = to_delete->next_queen;
		 free(to_delete);
	}
}

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


static void set_pool_parameters(int thread_num,int queue_size,int flags){
	global_variables.thread_count=thread_num;
	global_variables.queue_size=queue_size;
	global_variables.flags=0;
}

/*=====================================================================================================================*/




/*======================================================FUNCTION PROTOTYPE=============================================*/
void init_solv(void);
char safe_two_queens(struct queen* queen_prev, struct queen* queen_next);
char safe_sequence(struct queen* queen, struct queen* next_queen);
struct queen * update_sequence(struct queen * queen_list, struct queen* new_queen);
tree_node * update_child(tree_node * root, int val);
int Update_solution(struct  queen * queen);
int funct_solution_seq(void * in_variables);
void funct_solution_thread(void * in_variables);


void (* fun_ptr)(void*)=funct_solution_thread;

/*=====================================================================================================================*/
void init_solv(void){
	init_qarray();
	global_variables.solv_tree=createTree();
	//init_queenSeq();
#if THREADS	
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutexqueen, &Attr);
	global_variables.thpool = threadpool_create(global_variables.thread_count,global_variables.queue_size,global_variables.flags);
#endif	
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
	int nsolutions = global_variables.nsolv_ix.get();
	init_list(nsolutions);
	while(queen!=NULL){
		insert_element_to_list(nsolutions,queen->row,queen->column);
		queen= queen->next_queen;
	}
#endif
	global_variables.nsolv_ix.increment();
	return 1;
}



int funct_solution_seq(void * in_variables){
	/*cast the input arguments*/
	struct passing_variables variables = *(struct passing_variables *)in_variables;
	//tree_node * root=(tree_node*) malloc(1 * sizeof(tree_node));
	tree_node * root = variables.in_root;
	struct queen* local_queen_seq = variables.partial_seq;//Clone_queen_list(variables.partial_seq);
	long _level=variables.in_level;
	long _Nqueens=variables.in_Nqueens;
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
			funct_solution_seq(&variables_out);
		}
			/*once we send the copy for all childeren we can delete our local copy for safe memory*/
		clear_queen_list(&local_queen_seq);
		return 1;
	}else if(_level != _Nqueens){
		/*discart solution*/
		/*not solution was found then clear the list*/
		clear_queen_list(&local_queen_seq);
		return 0;
	}
	
return 1;	
}



void funct_solution_thread(void * in_variables){

struct passing_variables variables;	
struct queen* local_queen_seq;
tree_node * root;
long _level;
long _Nqueens;
int new_solv=0;	
int rq_split=0;	
int _split_ratio=0;	
int _ldeep=0;
unsigned int i;	
struct queen temp_queen;
	
/*casting the input arguments*/
	variables = *(struct passing_variables *)in_variables;
	//tree_node * root=(tree_node*) malloc(1 * sizeof(tree_node));
	root = variables.in_root;
	local_queen_seq = Clone_queen_list(variables.partial_seq);
	_level=variables.in_level;
	_Nqueens=variables.in_Nqueens;
	rq_split=variables.set_split;
	_ldeep=variables.deep_level;
	_split_ratio=variables.maximum_split;

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
				if(safe_sequence(local_queen_seq,&temp_queen)==1)
				{
					new_solv=1;
					update_child(root,i);
				}
			}
		}else{
			/*the algorithm find a solution*/
			/*critical seccion satart lock()*/

			lock();
			Update_solution(local_queen_seq);
			//counter.increment();
			//printf("%u\n",counter.get());
			clear_queen_list(&local_queen_seq);
			//pthread_exit(NULL);
			unlock();
			return;
		}
	}
/*evaluate if there is or not a new solution */	
	if(new_solv){
		/*in case of new solution... a new task is attached to the thread_pool y exit form the program*/
			int next_num_thread = root->child_num;//number of children form the previous computation.
			passing_variables * variables_split_out[next_num_thread-1]; //in  case to split at least child_num-1 will have done; 
			passing_variables *variables_out=new passing_variables();
			//struct passing_variables *variables_out=(passing_variables*)malloc(1 * sizeof(passing_variables)); //number of new task.
			//struct queen * tx_sequence_copy;
			int index=0;
			//int rc;/*for catch thread creation problems*/
			//tx_sequence_copy= //commun TX copy for all the childrens in case of more than on

			//if(next_num_thread > 1 && threadpool_threads_idle(global_variables.thpool) && 0	){ //Thradeoff optimizations.
			//	(variables_out)->in_level=_level;
			//	(variables_out)->in_Nqueens=_Nqueens;
			//	(variables_out)->partial_seq=Clone_queen_list(local_queen_seq);
			//	(variables_out)->in_root=root->child[0];
			//	rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(variables_out),0);
			//	
			//}

				for(i=0; i < next_num_thread; i++){
					(variables_out)->partial_seq=new queen();
					(variables_out)->in_root=new tree_node();
					(variables_out)->in_level=_level;
					(variables_out)->in_Nqueens=_Nqueens;
					//(variables_out)->partial_seq=local_queen_seq;
					(variables_out)->set_split=rq_split;
					(variables_out)->partial_seq=Clone_queen_list(local_queen_seq);	
					//(variables_out)->maximum_split =_split_ratio;
					(variables_out)->deep_level=_ldeep+1;
					//(variables_out)->in_root=root->child[i];		
					(variables_out)->in_root=root->child[i];
					if(rq_split && i < next_num_thread && (_ldeep == _split_ratio) && _split_ratio!=0){
						/*the solution rich the deep_threshold, then we have to delive the sequential to other thread and jump to the next
						  child*/
						  /*create a new object which contain the current data*/
						  //(variables_out)->partial_seq=local_queen_seq;
						  //	(variables_out)->in_root=root->child[i];
						  //(variables_out)->partial_seq=Clone_queen_list(local_queen_seq);	
						  (variables_out)->deep_level=0;
						  variables_split_out[index]=variables_out;
						  /*delagate the remain computation to other thread*/
							threadpool_add(global_variables.thpool,fun_ptr,(void*)(variables_split_out[index]),0);
						  _ldeep=0;
						  index++;
						  //return;
						  /*and then return*/
						  //return;
					}else{
						/* if not split the algorithm simple continue recursivelly to 
						search more solutions*/
						funct_solution_thread((void*)(variables_out));
						_ldeep=0;
						
					}
					
				}
/*once the next execution is setup we delete variables in order to reduce the memory consuption*/
			/*once we send the copy for all childeren we can delete our local copy for safe memory*/
		clear_queen_list(&local_queen_seq);
		//deep_level=0;
		//free(variables_out);
		return;						
	}else if(_level != _Nqueens){
/*not solution was found then delete the local variables and exit for the execution*/
		clear_queen_list(&local_queen_seq);
		/*critical Seccion unlook*/
		return;
	}
}


/*=====================================================================================================================*/

/*main def*/

int main(int argc, char *argv[]){
char* p;
int rc;
 struct timeb t1, t2;
int i;
int j;
double maximum_split;
int index;
struct passing_variables *start_variables;
struct passing_variables *array_variables;
		if (argc != 3){
					puts("you must to specified the maximum number of threads and the number of queens to evaluate");
		}
		int num_threads = strtol(argv[1], &p, 10);
		int queen_number= strtol(argv[2], &p, 10);
		
		if(queen_number<=3){
			PRINTF("Impossible to find a solution for (N=%u)\n",queen_number);
			return 0;
		}
		
		if(num_threads < 1){
			PRINTF("Impossible to find a solution for (Nthreads=%d)\n",num_threads);
			return 0;
		}
		
/**********************************************************************************************************************/
PRINTF("Starting the computation of Nqueens with (N=%u) queens and (T=%d)\n",queen_number,num_threads);		

set_pool_parameters(num_threads,4000,0);
init_solv();
maximum_split=((queen_number * queen_number) - (3*queen_number) + 2);
global_variables.solv_tree=createTree_num(-1);
if(num_threads<=queen_number){
	start_variables= (passing_variables*)malloc(queen_number * sizeof(passing_variables));
	ftime(&t1);
	for(i=0;i<queen_number;i++){
		(start_variables+i)->in_root=createTree_num(i);
		(start_variables+i)->in_level=0;
		(start_variables+i)->in_Nqueens=queen_number;
		(start_variables+i)->set_split=0;
		(start_variables+i)->maximum_split=0;
		(start_variables+i)->partial_seq=NULL;
	rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(start_variables+i),0);
		if(rc!=0){
			PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
			exit(-1);
		}
	}
thpool_wait(global_variables.thpool);
ftime(&t2);
}else{
	/*start a subdivision*/
	double thold=round((double)(queen_number * queen_number) / (double)num_threads)-1;
	if(thold<=1)thold=queen_number/2;
	if(num_threads >= maximum_split){
		array_variables=(passing_variables*)malloc(maximum_split * sizeof(passing_variables));
		struct queen * sent_seq=(struct queen*)malloc(maximum_split * sizeof(struct queen));
		/*split the frst level of the three*/
		index=0;
		ftime(&t1);
			for(i=0;i < queen_number; i++){
				for(j=0; j<queen_number; j++){
					if(abs(i-j)>=2){
						(sent_seq+index)->row=i;
						(sent_seq+index)->column=0;
						(sent_seq+index)->next_queen=NULL;
						(sent_seq+index)->tail=NULL;

						(array_variables+index)->in_Nqueens=queen_number;
						(array_variables+index)->in_level=1;
						(array_variables+index)->set_split=1;
						(array_variables+index)->maximum_split=thold;
						(array_variables+index)->deep_level=1;				
						(array_variables+index)->partial_seq=(sent_seq+index);
						(array_variables+index)->in_root=createTree_num(j);

						rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(array_variables+index),0);
							if(rc!=0){
								PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
								exit(-1);
							}
						index++;
					}

				}
			}
/*wait*/			
thpool_wait(global_variables.thpool);
ftime(&t2);
		}else{			
			int n_extremes=0;
			int	midle=0;
			
			if((midle=ceil((float)num_threads/(float)(queen_number-3)))>(queen_number-2)){
				midle=queen_number-2;
				n_extremes=ceil(abs((float)num_threads-((float)(queen_number-3)*(float)(queen_number-2)))/(float)(queen_number-2));
				
			}
			/*split the secuence*/
		array_variables=(passing_variables*)malloc(maximum_split * sizeof(passing_variables));
		struct queen * sent_seq=(struct queen*)malloc(maximum_split * sizeof(struct queen));
			/*1. split the midle*/
			index=0;
			int end_split=0;
			ftime(&t1);
			for(i=1;i <= queen_number-2; i++){
				for(j=0;j<queen_number;j++){
					if(i <= midle){
						if(abs(i-j)>=2){
							(sent_seq+index)->row=i;
							(sent_seq+index)->column=0;
							(sent_seq+index)->next_queen=NULL;
							(sent_seq+index)->tail=NULL;
						
							(array_variables+index)->in_Nqueens=queen_number;
							(array_variables+index)->in_level=1;
							(array_variables+index)->set_split=1;
							(array_variables+index)->deep_level=1;
							(array_variables+index)->maximum_split=thold;
							(array_variables+index)->partial_seq=(sent_seq+index);
							(array_variables+index)->in_root=createTree_num(j);
							
							rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(array_variables+index),0);
							if(rc!=0){
								PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
								exit(-1);
							}
						index++;
						}	
					}else {end_split=1; break;}
				}if(i <= queen_number-2 &&end_split==1 ){
					
					(array_variables+index)->in_root=createTree_num(i);
					(array_variables+index)->in_level=0;
					(array_variables+index)->set_split=1;
					(array_variables+index)->deep_level=0;
					(array_variables+index)->maximum_split=thold;
					(array_variables+index)->in_Nqueens=queen_number;
					(array_variables+index)->partial_seq=NULL;
							
							rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(array_variables+index),0);
							if(rc!=0){
								PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
								exit(-1);
							}
						index++;			
				}
			}
			
			/*2.split the extrems*/
			int split=n_extremes;
			if(n_extremes){
				i=0;
				do{
					for(j=0;j<queen_number;j++){
						if(abs(i-j)>=2){
							(sent_seq+index)->row=i;
							(sent_seq+index)->column=0;
							(sent_seq+index)->next_queen=NULL;
							(sent_seq+index)->tail=NULL;
							
							(array_variables+index)->in_Nqueens=queen_number;
							(array_variables+index)->in_level=1;
							(array_variables+index)->partial_seq=(sent_seq+index);
							(array_variables+index)->set_split=1;
							(array_variables+index)->maximum_split=thold;
							(array_variables+index)->deep_level=1;
							(array_variables+index)->in_root=createTree_num(j);
							
							rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(array_variables+index),0);
							if(rc!=0){
								PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
								exit(-1);
							}
							index++;
						}
						
					}
				split--;
				i=queen_number-1;
				}while(split);
			
				if(n_extremes < 2){

						(array_variables+index)->in_Nqueens=queen_number;
						(array_variables+index)->in_level=0;
						(array_variables+index)->partial_seq=NULL;
						(array_variables+index)->set_split=1;
						(array_variables+index)->deep_level=0;
						(array_variables+index)->maximum_split=thold;
						(array_variables+index)->in_root=createTree_num(queen_number-1);
						
						rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(array_variables+index),0);
							if(rc!=0){
								PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
								exit(-1);
							}
					index++;		
				}		
				
			}else{
				i=0;
				do{
						(array_variables+index)->in_Nqueens=queen_number;
						(array_variables+index)->in_level=0;
						(array_variables+index)->partial_seq=NULL;
						(array_variables+index)->set_split=1;
						(array_variables+index)->deep_level=0;
						(array_variables+index)->maximum_split=thold;
						(array_variables+index)->in_root=createTree_num(i);
						
						rc = threadpool_add(global_variables.thpool,fun_ptr,(void*)(array_variables+index),0);
							if(rc!=0){
								PRINTF("ERROR: return code from thpool_add_work() is %d\n", rc);
								exit(-1);
							}
							
					index++;		
					i=queen_number-1;		
					split++;
				}while(split < 2);
				
			}
thpool_wait(global_variables.thpool);		
ftime(&t2);
		}
}

//time(&t2);
printf("thread_ending\n");
int nSolutions=
#if STORE_RESULT
get_queen_array_size();
#else
global_variables.nsolv_ix.get();
#endif////nsolv_ix;
printf("num of solutions: %u\n",nSolutions);
//free(start_variables);
printResults(&t1, &t2);
		
return 1;		
}
