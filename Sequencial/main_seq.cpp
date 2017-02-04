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


#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#include "Seq_Nqueens.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#endif

#define PRINT_BOARD 1

using namespace std;


static void print_Results_mllisec(struct timeb * pt1, struct timeb * pt2){
    double secs;

                      double secs_t1 = ((double)(pt1->time)+((double)pt1->millitm/1000));
                      double sec_t2 = ((double)(pt2->time)+((double)pt2->millitm/1000));
                      secs = (sec_t2 - secs_t1);
                      std::cout<<"Calculations took second: "<< secs << "\n";
                      std::cout<< secs << "\n";
}





int main(int argc, char *argv[])
{

	assert(argc>1);
		int queen_number = atoi(argv[1]);

		if(queen_number<=3){
		  std::cout<<"Impossible to find a solution for (N= "<<queen_number<<")\n ";
		  return 0;
		}

/**************************************************************************************************************************************************************************************/
std::cout<<"Starting the computation of Nqueens with (N= "<<queen_number<<")\n";
struct timeb t1,t2;
int medle=0;
int results=0;
Chess_Board * Board= new Chess_Board(queen_number);
if(queen_number%2)medle=((queen_number+1)/2)-1;

ftime(&t1);
Nqueens (Board,1,0,queen_number,0,queen_number,medle,&results,0);
ftime(&t2);
cout<<"the computation ends with "<< 2* results << " results \n";
print_Results_mllisec(&t1,&t2);

}
