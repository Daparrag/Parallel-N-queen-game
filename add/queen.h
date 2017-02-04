#ifndef QUEEN_HEADER
#define QUEEN_HEADER


#include <iostream>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cstdlib>


struct queen{
	int row;
	int column;
	int size;
	int list_id;
	struct queen *next_queen;
	struct queen * tail;
	queen(int in_row, int column): row(in_row),column(column),size(0),list_id(0),next_queen(NULL),tail(NULL) {}
}; 


class Board{
public:
	int size;
	 std::vector<int>control_column;
	 std::vector< std::vector<int> >board_table;
	  Board(int in_size);  // This is the constructor
	  Board(int in_size, int initx, int intity);//constructor 2
	  char Complete_Computation();//used for end the computation;
	  char isSafe(int,int);
	 
};



void init_qarray(void);
int is_empty(int list_id);
char init_list(int list_id);
int get_link_size(int list_id);
struct queen * get_last(int list_id);
struct queen * gmet_first(int list_id);
struct queen * get_element_value_from_list(int list_id , int position);
void insert_element_to_list(int list_id,int Newqueen_row,int Newqueen_column);
int remove_element_from_list(int list_id,int queen_position);
int clear_list(int list_id);
int clear_array_entry(int array_index);
void print_queen_list(int list_id);
int get_queen_array_size(void);

#endif/*end LIST_HEADER*/


