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
#include <pthread.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


#include "Chess_Board.h"
#include "threadpool.h"

#define DEBUG 0

int *results=new int(0);

struct variables{
Chess_Board *Chess_Table;
int x;
int init_Row;
int end_Row;
int init_Col;
int end_Col;
int depth; 
int index;
int midle;
 bool print;
 variables(void *_var){
	struct variables variables_temp =*(struct variables *)_var;
	Chess_Table=new Chess_Board(*(variables_temp.Chess_Table));
	x=variables_temp.x;
	init_Row=variables_temp.init_Row;
	end_Row=variables_temp.end_Row;
	init_Col=variables_temp.init_Col;
	end_Col=variables_temp.end_Col;
	depth=variables_temp.depth;
	midle=variables_temp.midle;
	index=variables_temp.index;
	print=variables_temp.print;
 }
 
variables(Chess_Board *_Chess_Table,int _x,int _init_Row,int _end_Row,int _init_Col,int _end_Col,int _depth,int _midle,bool _print):
 x(_x),Chess_Table(_Chess_Table),init_Row(_init_Row),end_Row(_end_Row),init_Col(_init_Col),end_Col(_end_Col),depth(_depth),midle(_midle),print(_print)
 { index=0;}
 
};


pthread_mutex_t mutexqueen;
pthread_mutexattr_t Attr;
threadpool_t *thpool;

struct timeb t1, t2;

/**************************************************************************************/
void init_solv(int num_threads,int buff_size, int  flags);
static void lock(void);
static void unlock(void);
void Update_solution(int * results);
void Nqueen_fthread(void * in_variables);
static void print_Results_mllisec(struct timeb * pt1, struct timeb * pt2);
void (* fun_ptr)(void*)=Nqueen_fthread;
/**************************************************************************************/

void init_solv(int num_threads,int buff_size, int  flags){
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutexqueen, &Attr);
	thpool = threadpool_create(num_threads,buff_size,flags);
}



static void lock(void){
	int rc;
	rc = pthread_mutex_lock (&mutexqueen);
	if(rc!= 0){
		std::cout<<"ERROR: processing the mutex_lock" << "\n";
	}
}

static void unlock(void){
	int rc;
	rc = pthread_mutex_unlock(&mutexqueen);
	if(rc!= 0){
		std::cout<<"ERROR: processing the mutex_unlock" << "\n";
	}
}



void Update_solution(void){
	lock();
		*results=*results+1;
	unlock();
	
}

static void print_Results_mllisec(struct timeb * pt1, struct timeb * pt2){
    double secs;
	double secs_t1 = ((double)(pt1->time)+((double)pt1->millitm/1000));
	double sec_t2 = ((double)(pt2->time)+((double)pt2->millitm/1000));
	secs = (sec_t2 - secs_t1);
	std::cout<<"Calculations took second: "<< secs << "\n";
    std::cout<< secs << "\n";
}



void Nqueen_fthread(void * in_variables){
struct variables input_var=*(struct variables *)in_variables;
std::vector<variables> split_var;
/* initialize random seed: */
  //srand (time(NULL));
/*****************************Here we start the computation *******************************/
int i;
	if(input_var.Chess_Table->complete()){
			Update_solution();
			if(input_var.print){
				input_var.Chess_Table->PrintBoard();
				input_var.Chess_Table->PrintVector(0,1);
			}
			return;
	}else if(input_var.x==1){
		int range=input_var.midle;
		if(input_var.midle==0){range=(input_var.Chess_Table->size)/2;}
			for(i=input_var.init_Row; i < input_var.end_Row; i++){
				if(input_var.Chess_Table->Valid_Op(i,input_var.end_Col) && (input_var.init_Row < input_var.end_Row) && (input_var.init_Col < input_var.end_Col)){
					input_var.Chess_Table->mark_Solution(i,input_var.init_Col,1);
#if DEBUG
					//input_var.Chess_Table->PrintVector(0,0);
					//input_var.Chess_Table->PrintVector(2,0);
					//input_var.Chess_Table->PrintVector(1,0);
#endif				
					int Init_RowCopy=input_var.init_Row;
					int End_RowCopy=input_var.end_Row;
					
					input_var.init_Col=input_var.init_Col+1; /*go to the next column*/
					input_var.init_Row=0;/*setup the next row interval minimum val*/
					input_var.end_Row=input_var.Chess_Table->size;/*setup the next row interval maximum val*/
					input_var.x=0;/**/
					Nqueen_fthread(&input_var);	
					input_var.x=1;/**/
					input_var.init_Col=input_var.init_Col-1;/*go back to the previous column*/
					input_var.init_Row=Init_RowCopy;/*return to the original row values*/
					input_var.end_Row=End_RowCopy;/*return to the original row values*/
					input_var.Chess_Table->mark_Solution(i,input_var.init_Col,0);
				}
			}
			
			if(i==input_var.midle && input_var.midle!=0){
				input_var.Chess_Table->mark_Solution(i,0,1);	
				input_var.init_Col=1;
				for(i=input_var.midle+1; i < input_var.Chess_Table->size; i++){
					if(input_var.Chess_Table->Valid_Op(i,input_var.init_Col) && (input_var.init_Row < input_var.end_Row) && (input_var.init_Col < input_var.end_Col)){
						input_var.Chess_Table->mark_Solution(i,input_var.init_Col,1);
#if DEBUG
					//input_var.Chess_Table->PrintVector(0,0);
					//input_var.Chess_Table->PrintVector(2,0);
					//input_var.Chess_Table->PrintVector(1,0);
					
					
#endif				
					int Init_RowCopy=input_var.init_Row;
					int End_RowCopy=input_var.end_Row;
					input_var.init_Col=input_var.init_Col+1; /*go to the next column*/
					input_var.init_Row=0;/*setup the next row interval minimum val*/
					input_var.end_Row=input_var.Chess_Table->size;/*setup the next row interval maximum val*/
					input_var.x=0;/**/
					Nqueen_fthread(&input_var);
					input_var.x=1;/**/
					input_var.init_Col=input_var.init_Col-1; /*go back to the previous column*/
					input_var.init_Row=Init_RowCopy;/*return to the original row values*/
					input_var.end_Row=End_RowCopy;/*return to the original row values*/
					input_var.Chess_Table->mark_Solution(i,input_var.init_Col,0);
					}
				}
				
			}
	}else{
		for(int i=input_var.init_Row; i < input_var.Chess_Table->size; i++){
			if(input_var.Chess_Table->Valid_Op(i,input_var.init_Col) && (input_var.init_Row < input_var.end_Row)&& (input_var.init_Col < input_var.end_Col)){
				input_var.Chess_Table->mark_Solution(i,input_var.init_Col,1);
#if DEBUG
					//input_var.Chess_Table->PrintVector(0,0);
					//input_var.Chess_Table->PrintVector(2,0);
					//input_var.Chess_Table->PrintVector(1,0);
					//input_var.Chess_Table->PrintVector(3,0);
					//input_var.Chess_Table->PrintVector(4,0);
					//std::cout<<input_var.index<<"\n";
#endif	 			
					int tmp_index=input_var.index;
					int tmp_column=input_var.init_Col; 
					input_var.init_Col=input_var.init_Col+1; /*go to the next column*/

					if((input_var.depth==input_var.index) && (input_var.depth!=0)){
						/*split the computation from then next column*/
						
						input_var.index=0;
						struct variables send_var= new variables(&input_var);/*we clone the variables to asignate to the next thread*/
						int rc = threadpool_add(thpool,fun_ptr,&send_var,sizeof(variables),0);/*assign the remaining part to other thread*/
							if(rc!=0){
								std::cout<< "ERROR: return code from thpool_add_work() is " << rc << "\n";
								
								exit(-1);
							}
							input_var.depth=0;
							//tmp_index=tmp_index+1;

					}else{
						/*if there is not the time to split it is time to continue recursivelly */

						input_var.index = input_var.index+1;
						Nqueen_fthread(&input_var);
					}	
				input_var.index=0;	
				input_var.init_Col=tmp_column; /*go back to the previous column*/	
				input_var.Chess_Table->mark_Solution(i,input_var.init_Col,0);
			}
			//input_var.depth=0;
		}
		
	}
}




void emitter(int depth, int midle, int is_even, int x,int init_row,int end_row,int init_column,int end_column,Chess_Board *Chess_Table,int first_split){
	int i;
	if(x==1){
		for(i=0; i < midle; i++){
			if(Chess_Table->Valid_Op(i,init_column) && (init_row < end_row) && (init_column < end_column)){
					Chess_Table->mark_Solution(i,init_column,1);
					emitter(depth,midle,is_even,0,0,end_row,(init_column+1),end_column,Chess_Table,first_split);
					Chess_Table->mark_Solution(i,init_column,0);
			}
		}

		if(!is_even && (i == midle)){
			Chess_Table->mark_Solution(i,0,1);
			emitter(depth,midle,is_even,0,(i+1),end_row,(init_column+1),end_column,Chess_Table,first_split);

		}
		
	}else{
		for(int i=init_row; i < end_row; i++){//Will be evaluate all the rows
			if(Chess_Table->Valid_Op(i,init_column) && (init_row < end_row)&& (init_column < end_column)){
			Chess_Table->mark_Solution(i,init_column,1);
				if((depth==init_column)){
					/*split the computation*/
					if(first_split){
					first_split=0;
					}

#if DEBUG
					Chess_Table->PrintVector(0,0);
					Chess_Table->PrintVector(2,0);
					Chess_Table->PrintVector(1,0);
					std::cout<<"split:"<<midle<<" "<<depth<<" "<<init_column<<" "<<init_row<<" "<< end_row<<"\n";
#endif					
					Chess_Board *chess_table_cp = new Chess_Board(*(Chess_Board*)Chess_Table);
					variables * Start_var=new variables(chess_table_cp,0,0,end_row,(init_column+1),end_column,0,midle,0);
					int rc = threadpool_add(thpool,fun_ptr,Start_var,sizeof(variables),0);/*assign the remaining part to other thread*/
							if(rc!=0){
								std::cout<< "ERROR: return code from thpool_add_work() is " << rc << "\n";
								
								exit(-1);
							}			
				}else{
#if DEBUG
					Chess_Table->PrintVector(0,0);
					Chess_Table->PrintVector(2,0);
					Chess_Table->PrintVector(1,0);
					std::cout<<"recursive:"<<midle<<" "<<depth<<" "<<init_column<<" "<<init_row<<" "<< end_row<<"\n";
#endif					
					emitter(depth,midle,is_even,0,0,end_row,(init_column+1),end_column,Chess_Table,first_split);				
				}
				Chess_Table->mark_Solution(i,init_column,0);	
			}
		}
	}
}
/*=====================================================================================================================*/

/*main def*/

int main(int argc, char *argv[]){
int i;	
char* p;
int depth;
int medle;
int temp_medle;
int index;

int even;
//*results=0;


		if (argc != 3){
					puts("you must to specified the maximum number of threads and the number of queens to evaluate");
		}
		int num_threads = strtol(argv[1], &p, 10);
		int queen_number= strtol(argv[2], &p, 10);
		
		if(queen_number<=3){
			std::cout<<"Impossible to find a solution for N: "<<queen_number<<"\n";
			return 0;
		}
		
		if(num_threads < 1){
			std::cout<<"Impossible to find a solution for Nthreads: "<<num_threads<<"\n";
			return 0;
		}
			
/**********************************************************************************************************************/
std::cout<<"Starting the computation of Nqueens with N: "<< queen_number << " and Threads: "<< num_threads<<"\n";
init_solv(num_threads,4000000,0);
threadpool_set_wait(thpool,1);

	if(queen_number%2){
		temp_medle = ((queen_number+1)/2)-1;
		medle=temp_medle;
		even=0;
	}else{
		temp_medle =	((queen_number)/2);
		medle=0;
		even=1;
	}

if(num_threads<=temp_medle){
Chess_Board *chess_table[temp_medle];	
variables * Start_var[temp_medle];
	ftime(&t1);		
		for (i=0;i<temp_medle;i++){
			chess_table[i]=new Chess_Board(queen_number);
			Start_var[i] = new variables(chess_table[i],1,i,i+1,0,queen_number,0,medle,0);
		int rc = threadpool_add(thpool,fun_ptr,(void*)(Start_var[i]),sizeof(variables),0);
			if(rc!=0){
				std::cout<< "ERROR: return code from thpool_add_work() is " << rc << "\n";					
				exit(-1);
			}	
		}
threadpool_set_wait(thpool,0);		
thpool_wait(thpool);		
ftime(&t2);
}else{
	/*in this case is necessary define How far goes one thread in the computation before the delegate the result to a new thread.*/
depth=ceil(log(num_threads)/log(queen_number));
//std::cout<<depth<< "\n";
Chess_Board *chess_table= new Chess_Board(queen_number);
ftime(&t1);
emitter(depth,temp_medle,even,1,0,(queen_number),0,(queen_number),chess_table,1);
threadpool_set_wait(thpool,0);
thpool_wait(thpool);
ftime(&t2);	
//print_Results_mllisec(&t1,&t2);	
}	
int Solutions=*results;
std::cout<<"the computation ends with "<< 2* Solutions << " results \n";
print_Results_mllisec(&t1,&t2);
/*variables(Chess_Board *_Chess_Table,int _x,int _init_Row,int _end_Row,int _init_Col,int _end_Col,int _depth,int _midle,int *_rest,bool _print)*/	
/*int depth, int midle, int is_even, int x,int init_row,int end_row,int init_column,int end_column,Chess_Board *Chess_Table*/
	
}