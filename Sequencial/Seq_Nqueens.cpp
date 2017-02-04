/*   Board Size:       Number of Solutions to          Time to calculate
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
 *    19                         4968057848                  02:31:24
 *    20                        39029188884                  20:35:06
 *    21                       314666222712                 174:53:45
 *    22                      2691008701644                     ?
 *    23                     24233937684440                     ?
 *    24                                  ?                     ?
 */

#include "Seq_Nqueens.h"
#include <vector>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <stdio.h>


#define DEBUG 0
#if DEBUG
#include <stdio.h>
#endif

#define PRINT_BOARD 1

using namespace std;

void Nqueens (Chess_Board *Chess_Table, int x, int init_Row, int end_Row, int init_Col, int end_Col, int midle,int *rest, bool print)
{
	int i;
	if(Chess_Table->complete()){
		*rest=*rest+1;
#if PRINT_BOARD
		if(print){
		Chess_Table->PrintBoard();
		Chess_Table->PrintVector(0,1);
		}
#endif
		return;
	}else if(x==1){
		int range=midle;
		if(midle==0){range=(Chess_Table->size)/2;}
			for(i=init_Row; i < range; i++ ){/*Will be evaluated only the fist three row*/
				if(Chess_Table->Valid_Op(i,init_Col) && (init_Row < end_Row) && (init_Col < end_Col)){
					Chess_Table->mark_Solution(i,init_Col,1);
#if DEBUG
					Chess_Table->PrintVector(0,0);
					Chess_Table->PrintVector(2,0);
					Chess_Table->PrintVector(1,0);
#endif
					Nqueens(Chess_Table,0,0,end_Row,(init_Col+1),end_Col,midle,rest,print);
					Chess_Table->mark_Solution(i,init_Col,0);
				}
			}

	if(i==midle && midle!=0){
		Chess_Table->mark_Solution(i,0,1);
		init_Col=1;
		for(i=midle+1; i < Chess_Table->size; i++){
			if(Chess_Table->Valid_Op(i,init_Col) && (init_Row < end_Row) && (init_Col < end_Col)){
				Chess_Table->mark_Solution(i,init_Col,1);
#if DEBUG
					Chess_Table->PrintVector(0,0);
					Chess_Table->PrintVector(2,0);
					Chess_Table->PrintVector(1,0);
#endif
					Nqueens(Chess_Table,0,0,end_Row,(init_Col+1),end_Col,midle,rest,print);
					Chess_Table->mark_Solution(i,init_Col,0);
			}
		}
	}


	}else{
		/*for(int i=0; i < Solution->size; i++){*/
				for(int i=init_Row; i < Chess_Table->size; i++){//Will be evaluate all the rows
						if(Chess_Table->Valid_Op(i,init_Col) && (init_Row < end_Row)&& (init_Col < end_Col)){
							Chess_Table->mark_Solution(i,init_Col,1);
#if DEBUG
							Chess_Table->PrintVector(0,0);
							Chess_Table->PrintVector(2,0);
							Chess_Table->PrintVector(1,0);
#endif
							Nqueens(Chess_Table,0,0,end_Row,(init_Col+1),end_Col,midle,rest,print);
							Chess_Table->mark_Solution(i,init_Col,0);
						}
				}
		}
}

