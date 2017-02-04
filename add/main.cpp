#include "queen.h"
#include <vector>
#include <iostream>
#include <cstdlib>



using namespace std;
int results=0;
char get_result(Board Solution,int x,int start_y, int end_y){
int i,j;

	if (Solution.Complete_Computation()){
		results=results+1;
		return results;
	}else{
		for(j=start_y;j<end_y;j++)
		for(i=0;i<Solution.size;i++){
			if(Solution.isSafe(i,j)){
				Solution.control_column[j]=1;
				Solution.board_table[j][i]=1; //Is the queen could be alocate in the board we continue reursivelly to the next column
				get_result(Solution,x,(start_y+1),end_y);			
			}
			Solution.board_table[j][i]=0;
			Solution.control_column[j]=0;
		}

	}
}


int main(int argc, char *argv[])
{
	int i;
	int jave;
	std::vector<queen*>queen_arry;
	Board new_table(4);
	get_result(new_table,1,0,new_table.size);
	//Board new_table(4);
	printf("%d",results);
	jave = new_table.Complete_Computation();
	
	
	
	
	

	
	

	for(i=0;i<10;i++){
		struct queen* new_queen = new queen (i,0);
		queen_arry.push_back(new_queen);
		
	}
	
}
