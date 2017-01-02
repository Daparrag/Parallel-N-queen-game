#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

#include "tree.h"
#include "queen.h"
#include "Seq_queen.h"

#include <math.h>
#include <ff/allocator.hpp>
#include <ff/farm.hpp>
#include <stdio.h>

using namespace ff;
static ff_allocator ffalloc;

int n_workers;
int n_queens;
int Tresult;


typedef struct {
tree_node *in_root;
int in_level;
int in_Nqueens;
struct queen * partial_seq;

}ff_task_t;


struct Emitter:ff_node_t<ff_task_t>{ 

	int svc_init() {
		ffalloc.init();
        if (ffalloc.registerAllocator()<0) {
        error("Emitter, registerAllocator fails\n");
        return -1;
		}
	}
	

	ff_task_t* svc(ff_task_t* t){
		int maximum_split=((n_queens * n_queens) - (3*n_queens) + 2);
		
		
		if(n_workers <= n_queens){
			int i;
			
			ff_task_t * task = (ff_task_t*)ffalloc.malloc(n_queens * sizeof(ff_task_t));
			for (int i=0;i < n_queens; i++){
				(task+i)->in_Nqueens=n_queens;
				(task+i)->in_level=0;
				(task+i)->in_root=createTree_num(i);
				(task+i)->partial_seq=NULL;
				ff_send_out(task+i);
			}
			return ((ff_task_t*)FF_EOS);
		}else{
			/*we have to make a partition*/
			if(n_workers >= maximum_split){
			ff_task_t * task = (ff_task_t*)ffalloc.malloc(maximum_split * sizeof(ff_task_t));
			struct queen * sent_seq = (struct queen*)ffalloc.malloc(maximum_split * sizeof(struct queen));
			int i,j;
			int index=0;
				for(i=0;i < n_queens; i++){
					for(j=0; j<n_queens; j++){
						if(abs(i-j)>=2){
						(sent_seq+index)->row=i;
						(sent_seq+index)->column=0;
						(sent_seq+index)->next_queen=NULL;
						(sent_seq+index)->tail=NULL;
						
						(task+index)->in_Nqueens=n_queens;
						(task+index)->in_level=1;
						(task+index)->partial_seq=(sent_seq+index);
						(task+index)->in_root=createTree_num(j);
						ff_send_out(task+index);
						index++;
						}
					}
				}
			return ((ff_task_t*)FF_EOS);	
			}else{
				int i,j;
				int index;
				int n_extremes=0;
				int	midle=0;
				ff_task_t * task = (ff_task_t*)ffalloc.malloc(n_workers * sizeof(ff_task_t));
				struct queen * sent_seq = (struct queen*)ffalloc.malloc(n_workers * sizeof(struct queen));
			
				if((midle=ceil((float)n_workers/(float)(n_queens-3)))>(n_queens-2)){
					n_extremes=ceil(((float)n_workers-((float)(n_queens-3)*(float)((n_queens-2))))/((float)(n_queens-2)));
				}
			/*1. split the midle*/
				index=0;
				int end_split=0;
				for(i=1;i <= n_queens-2; i++){
					for(j=0;j<n_queens;j++){
						if(i <= midle){
							if(abs(i-j)>=2){
								(sent_seq+index)->row=i;
								(sent_seq+index)->column=0;
								(sent_seq+index)->next_queen=NULL;
								(sent_seq+index)->tail=NULL;
							
								(task+index)->in_Nqueens=n_queens;
								(task+index)->in_level=1;
								(task+index)->partial_seq=(sent_seq+index);
								(task+index)->in_root=createTree_num(j);
								ff_send_out(task+index);
								index++;
							}
						}else {end_split=1; break;}
					}if(i <= n_queens-2 &&end_split==1 ){
								(task+index)->in_root=createTree_num(i);
								(task+index)->in_level=0;
								(task+index)->in_Nqueens=n_queens;
								(task+index)->partial_seq=NULL;
								ff_send_out(task+index);
								index++;			
					}
				}
			
		/*2.split the extrems*/	
				int split=n_extremes;
				if(n_extremes){
					i=0;
					do{
						for(j=0;j<n_queens;j++){
							if(abs(i-j)>=2){
								(sent_seq+index)->row=i;
								(sent_seq+index)->column=0;
								(sent_seq+index)->next_queen=NULL;
								(sent_seq+index)->tail=NULL;
							
							
								(task+index)->in_Nqueens=n_queens;
								(task+index)->in_level=1;
								(task+index)->partial_seq=(sent_seq+index);
								(task+index)->in_root=createTree_num(j);
								ff_send_out(task+index);
								index++;
							}
						}
					split--;
					i=n_queens-1;	
					}while(split);
				
					if(n_extremes < 2){
						(task+index)->in_Nqueens=n_queens;
						(task+index)->in_level=0;
						(task+index)->partial_seq=NULL;
						(task+index)->in_root=createTree_num(n_queens-1);
						ff_send_out(task+index);
						index++;
					}
				}else{
					i=0;
					do{
						(task+index)->in_Nqueens=n_queens;
						(task+index)->in_level=0;
						(task+index)->partial_seq=NULL;
						(task+index)->in_root=createTree_num(0);
						ff_send_out(task+index);
						index++;
						split++;
						i=n_queens-1;		
					}while(split < 2);
				}
			return ((ff_task_t*)FF_EOS);	
			}
		}
	}
	
//void svc_end (){
//	ffalloc.free(task);
//	}
	
}emitter;

struct Worker:ff_node_t<ff_task_t>{
		
		
	 
	 ff_task_t* svc(ff_task_t* t){
		  //this variable will contain the partial result getting from the current task
        //int results=0;
		int solution;
		funct_solution_seq(t->in_root,t->partial_seq,t->in_level,t->in_Nqueens,&Tresult);
		solution=Tresult;
		//printf("num_sol %u ,Tresult %u \n",solution, Tresult);
		return ((ff_task_t*)GO_ON);
	 }
	 
	 
	
}worker;


int main(int argc, char** argv) {
char* p;	
time_t t1, t2;
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

init_solv();
ffalloc.init();

//std::vector<int>row_queens;
//int solu=0;
//row_queens.push_back(1);
//funct_solution_nonrecursive(n_queens,&solu,row_queens);
//printf("solu %u\n",solu);

//return 0;

 std::vector<std::unique_ptr<ff_node> > Workers;
 for(int i=0;i<n_workers;++i){ 
        Workers.push_back(make_unique<Worker>());
    }
	
  ff_Farm<ff_task_t> farm(std::move(Workers),emitter);
//removing default collector
    farm.remove_collector();
    farm.set_scheduling_ondemand();
//starting time
	ff::ffTime(ff::START_TIME);
	if (farm.run_and_wait_end()<0) error("running farm");
	std::cout<<"The solutions are:"<<Tresult<<"\n";	
	//stopping time
    ff::ffTime(ff::STOP_TIME);
// printing completion time
    std::cout << "Completion time: (MS)\n";
    std::cout << ff::ffTime(ff::GET_TIME) << std::endl;
	std::cout << "Completion time: (S)\n";
	std::cout << ff::ffTime(ff::GET_TIME)*(0.001) << std::endl;
//printing statistics	
	return 0;
}
