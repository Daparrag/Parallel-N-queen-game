#include <math.h>
#include <ff/farm.hpp>

using namespace ff;

int n_workers;
int n_queens;
int Tresult;


struct Task{
tree_node *in_root;
int in_level;
int in_Nqueens;
struct queen * partial_seq;
};

typedef struct Task ObTask;


/*===============================CONSTRUCTOR====================================*/





ObTask* new_Task(int _value) { 
  ObTask* p = malloc(sizeof(ObTask));
  p->
  p->value = value;
  return p;
}





struct Emitter:ff_node_t<Task>{ 
	Task* svc(Task* t){
		if(n_queens >= n_workers){
			int i;
			for (int i=0;i < n; i++){
				Task * to_task=new Task(i);
			}
			
		}
}

struct Worker:ff_node_t<Task>{
	
}worker;