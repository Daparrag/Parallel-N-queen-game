#ifndef BOARD_HEADER
#define BOARD_HEADER


#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cstdlib>


class Chess_Board{
private:
		bool *Row_Control;
		bool *Col_Control;
		bool *Diag_Control;
		bool *AntiD_Control;
public:
	int results;
	int size;
	int * Board;
	Chess_Board(int size);
	Chess_Board(volatile Chess_Board &obj);
	char Check_Solution(int row);
	char Valid(int row);
	char Valid_Op (int Row, int Col);
	void PrintVector (int type, int solv);
	void mark_Solution(int row,int column,bool flag);
	bool complete();
	void PrintBoard();
};








#endif/*end LIST_HEADER*/
