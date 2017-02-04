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
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <ff/allocator.hpp>
#include <ff/farm.hpp>
#include <stdio.h>

#include "Seq_Nqueens.h"

#define DEBUG 0

using namespace ff;
static ff_allocator ffalloc;




typedef struct ff_task_t{
Chess_Board *Chess_Table;
int x;
int init_Row;
int end_Row;
int init_Col; 
int end_Col;
int midle;
int depth;
int even;
bool print;
ff_task_t(Chess_Board *_Chess_Table,int _x, int _init_Row,int _end_Row
	,int _init_Col,int _end_Col, int _midle,int _even,int _depth,bool _print) :
	x(_x), Chess_Table(_Chess_Table), init_Row(_init_Row), end_Row(_end_Row)
	,init_Col(_init_Col), end_Col(_end_Col), depth(_depth), midle(_midle),print(_print),even(_even)
	{

	}

}ff_task_t;

int n_workers;
int n_queens;
int Results;






struct Worker:ff_node_t<ff_task_t>{
	ff_task_t* svc(ff_task_t* t){
		int solution=0;
		Nqueens (t->Chess_Table, t->x, t->init_Row, t->end_Row,t->init_Col, t->end_Col, t->midle,t->even, &solution, t->print);
		//std::cout<<"results: "<<solution<<"\n";
		Results=Results+(solution);
		return ((ff_task_t*)GO_ON);
	}
}worker;


struct Emitter:ff_node_t<ff_task_t>{

	int svc_init() {
		ffalloc.init();
        if (ffalloc.registerAllocator()<0) {
        error("Emitter, registerAllocator fails\n");
        return -1;
		}
		return 1;
	}


void emitter_split(int midle,int depth,int is_even,int x,int init_row,int end_row,int init_column,int end_column, Chess_Board *Chess_Table,int first_split){
int i;
	if(x==1){
		for(i=0; i < midle; i++){
			if(Chess_Table->Valid_Op(i,init_column) && (init_row < end_row) && (init_column < end_column)){
				Chess_Table->mark_Solution(i,init_column,1);
				emitter_split(midle,depth,is_even,0,0,end_row,(init_column+1),end_column,Chess_Table,first_split);
				Chess_Table->mark_Solution(i,init_column,0);
			}
		}

		if(!is_even && (i == midle)){
			Chess_Table->mark_Solution(i,0,1);
			emitter_split(midle,depth,is_even,0,0,end_row,(init_column+1),end_column,Chess_Table,first_split);
		}

	}else{
		for(int i=init_row; i < end_row; i++){//Will be evaluate all the rows
			if(Chess_Table->Valid_Op(i,init_column) && (init_row < end_row)&& (init_column < end_column)){	
				Chess_Table->mark_Solution(i,init_column,1);
				if((depth==init_column)){
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
					ff_task_t * Start_var=new ff_task_t(chess_table_cp,0,0,end_row,(init_column+1),end_column,midle,is_even,depth,0);
					ff_send_out(Start_var);
				}else{
#if DEBUG
					Chess_Table->PrintVector(0,0);
					Chess_Table->PrintVector(2,0);
					Chess_Table->PrintVector(1,0);
					std::cout<<"recursive:"<<midle<<" "<<depth<<" "<<init_column<<" "<<init_row<<" "<< end_row<<"\n";
#endif
					emitter_split(midle,depth,is_even,0,0,end_row,(init_column+1),end_column,Chess_Table,first_split);
				}
			Chess_Table->mark_Solution(i,init_column,0);
			}
			
		}
	}	
}

	ff_task_t* svc(ff_task_t* t){
		int midle;
		int even;
		int temp_medle;
		int depth;

		if(n_queens%2){
		temp_medle = ((n_queens+1)/2)-1;
		midle=temp_medle;
		even=0;
	}else{
		temp_medle =	((n_queens)/2);
		midle=0;
		even=1;
	}

	if(n_workers<=temp_medle){
		int i;
		Chess_Board *chess_table[temp_medle];	
		ff_task_t * Start_var[temp_medle];
		for (i=0;i< temp_medle;i++){
			chess_table[i]=new Chess_Board(n_queens);
			/**/
			Start_var[i]=new ff_task_t(chess_table[i],1,i,(i+1),0,n_queens,midle,even,0,0);
			//emitter_split(temp_medle,1,even,1,0,(n_queens),0,(n_queens), chess_table[i],1);
			ff_send_out(Start_var[i]);
		}
	
	}else{
		/*here is necesary split the computation as much as possible*/
		/*int midle,int depth,int is_even,int x,int init_row,int end_row,int init_column,int end_column, Chess_Board *Chess_Table,int first_split*/
		depth=ceil(log(n_workers)/log(n_queens));
		Chess_Board *chess_table= new Chess_Board(n_queens);
		emitter_split(temp_medle,depth,even,1,0,(n_queens),0,(n_queens), chess_table,1);


	}

return ((ff_task_t*)FF_EOS);	
	}
}emitter;

int main(int argc, char** argv) {
char* p;
		if (argc != 3){
					puts("you must to specified the maximum number of threads and the number of queens to evaluate");
		}
		n_workers = strtol(argv[1], &p, 10);
		n_queens= strtol(argv[2], &p, 10);
		
		if(n_queens<=3){
			printf("Impossible to find a solution for (N=%u)\n",n_queens);
			return 0;
		}
		
		if(n_workers < 1){
			printf("Impossible to find a solution for (Nthreads=%d)\n",n_workers);
			return 0;
		}

/**********************************************************************************************************************/
ffalloc.init();

std::vector<std::unique_ptr<ff_node> > Workers;
	for(int i=0;i<n_workers;++i){ 
    	    Workers.push_back(make_unique<Worker>());
    	}

ff_Farm<ff_task_t> farm(std::move(Workers),emitter);
farm.remove_collector();
farm.set_scheduling_ondemand();
std::cout<<"Starting the computation of Nqueens with N: "<< n_queens << " and Threads: "<< n_workers<<"\n";
ff::ffTime(ff::START_TIME);
	if (farm.run_and_wait_end()<0) error("running farm");
	std::cout<<"The solutions are:"<<2*Results<<"\n";	
 ff::ffTime(ff::STOP_TIME);
  //std::cout << "Completion time: (MS)\n";
    //std::cout << ff::ffTime(ff::GET_TIME) << std::endl;
	std::cout << "Completion time: (S)\n";
	std::cout << ff::ffTime(ff::GET_TIME)*(0.001) << std::endl;
return 0;	
}