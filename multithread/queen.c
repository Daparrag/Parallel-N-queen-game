#include "queen.h"

#define DEBUG 1
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

/*------------------------------MACROS----------------------------------------------*/
#define MEM_ARRAY(size) (struct queen**) malloc (sizeof(struct queen*) * size)
#define NEW_NODE_MEM (struct queen*) malloc (sizeof(struct queen))
#define REALLOC_MEM(size) (struct queen**)realloc(queen_array,(size)* sizeof(queen*))

/*------------------------------GLOBAL-VARIABLES-------------------------------------*/

struct queen **queen_array;
int array_size;

/*----------------------------STATIC FUCTIONS ---------------------------------------*/
static struct queen * get_head_element(int list_id);
static struct queen * get_tail_element(int list_id);
static struct queen * get_next_element(struct queen * ptr);


int get_queen_array_size(void){
	return array_size;
}


void init_qarray(void){
	array_size=0;
	queen_array = MEM_ARRAY(1);
}

/*is_empty: define if the list is empty*/
int is_empty(int list_id){
	return queen_array[list_id]->next_queen==NULL;
}

/*init_list: Instantiate the  the list*/
char init_list(int list_id){
	struct queen * new_queen=NEW_NODE_MEM;
	new_queen->size=0;
	new_queen->next_queen=NULL;
	array_size++;
	queen_array = REALLOC_MEM(array_size);
	queen_array[array_size-1]=new_queen;
	return 1;
}

/*get_link_size: return the size of the list*/
int get_link_size(int list_id){
	struct queen * prt_head;
	if((prt_head = get_head_element(list_id)))return prt_head->size;
	return -1;
}

/*get_link_size: return the last element of the list*/
struct queen * get_last(int list_id){
	return queen_array[list_id]->tail;
}

/*get_first::return the first element of the list*/
struct queen * get_first(int list_id){
	struct queen * prt_head = get_head_element(list_id);
		if(prt_head->next_queen!=NULL)return prt_head->next_queen;
		PRINTF("There is not elements in the list\n");
		return 0;
}



/*get_element_value_from_list::return a value of the list in a certain position*/
struct queen * get_element_value_from_list(int list_id , int position){
	struct queen * ptr = get_head_element(list_id);
	if(ptr->next_queen==NULL) return NULL;
	int list_size = ptr->size;
	ptr=ptr->next_queen;
	if(position>list_size){
		PRINTF("position Don't exit in the list\n");
		return NULL;
	}else{
		int i;
		for(/*int i=0;*/i=0;i<position;i++){
			ptr=ptr->next_queen;
		}
		return ptr;
	}

}

/*insert_element_to_list::insert a element to te corresponding list*/
void insert_element_to_list(int list_id,int Newqueen_row,int Newqueen_column){
	struct queen * new_queen = NEW_NODE_MEM;
	struct queen * ptr;
	struct queen * ptr_head;
	if(list_id > array_size){
		PRINTF("queen: queen can't be inserted\n");
		return;
	}else if ((ptr=get_head_element(list_id))==0){
		PRINTF("queen: queen can't be inserted\n");
		return;
	}
	/*insert the queen*/
	new_queen->row=Newqueen_row;
	new_queen->column=Newqueen_column;
	ptr_head=ptr;
	ptr->size+=1;
	if(ptr->next_queen==NULL){
		ptr->next_queen=new_queen;
		ptr->tail=new_queen;
	}else{
		ptr=ptr->tail;
		ptr->next_queen=new_queen;
		ptr_head->tail=new_queen;
	}
}


/*remove_element_from_list::remove a specific element to the list*/
int remove_element_from_list(int list_id,int queen_position){
	int i;
	int list_size;
	struct queen * ptr;
	struct queen * prev_ptr;
	struct queen * head_ptr;
	
	if(list_id > array_size){
		PRINTF("queen: the array Id inserted don't exist\n");
		return -1;
	}else if((ptr=get_head_element(list_id))==0 ||(head_ptr= get_head_element(list_id))==0 ){
		PRINTF("queen: error processing the list\n");
		return -1;
	}
	list_size=ptr->size;
	if(queen_position > list_size){
		PRINTF("queen: error the possition correspound to inexistent element in the list\n");
		return -1;
	}
		ptr=get_first(list_id);
		for(i=0;i<queen_position;i++){
			ptr=get_next_element(ptr);
			if(i==queen_position-1){
				prev_ptr=ptr;
			}
		}
	head_ptr->size-=1;
		if(ptr==get_first(list_id)){
			ptr->next_queen=NULL;
			ptr->tail=NULL;
			free(ptr);
		}else if(ptr->next_queen==NULL){
			prev_ptr->next_queen=NULL;
			head_ptr->tail=prev_ptr;
			free(ptr);
		}else{
			prev_ptr->next_queen=ptr->next_queen;
			free(ptr);
		}
		return 1;
}

int clear_list(int list_id){
	
	if(list_id > array_size){
		PRINTF("queen: the array id inserted don't exist\n");
		return -1;
	}
	
	struct queen * ptr;
	struct queen * next_ptr;
	
	int print_size;
	
	ptr = get_head_element(list_id);
	if(ptr->size==0){
		PRINTF("queen: queen list empty\n");
		return -1;
	}else{
		struct queen * fist_ptr=get_first(list_id);
		while(fist_ptr){
		next_ptr = fist_ptr->next_queen;
		free(fist_ptr);
		fist_ptr=next_ptr;
		}
		ptr->next_queen=NULL;
		ptr->tail=NULL;
		ptr->size=0;
	}
return 1;	
}

int clear_array_entry(int array_index){
	unsigned int i;
	if(array_index>array_size){
		PRINTF("queen: the array Id inserted don't exist\n");
		return -1;
	}
	
	if(clear_list(array_index)!=0){
		PRINTF("queen: error clearing the array_entry\n");
		return -1;
	}
	
	for(i=array_index;i<array_size-1;i++){
		queen_array[i]=queen_array[i-1];
	}
	
	/*Reallocate */
	array_size-=1;
	queen_array=REALLOC_MEM(array_size);
	return 1;
}



#ifdef DEBUG
/*remove_element_from_list::print all the elements of the list*/
void print_queen_list(int list_id){
	struct queen * ptr;
	int print_size;
	
	if(list_id>array_size){
		PRINTF("queen: the array Id inserted don't exist\n");
		return;
	}
	
	
	
	ptr = get_head_element(list_id);
	if(ptr->size==0){
		PRINTF("list: list empty\n");
		return;
	}
	int i;
	PRINTF("list elements : < ");
	print_size=ptr->size;
	ptr=ptr->next_queen;

	for(i=0;i<print_size;i++){
		if(i<print_size-1){
			PRINTF("(row: %u,column: %u),",ptr->row,ptr->column);
			ptr=ptr->next_queen;
		}
		else PRINTF("(row: %u,column: %u)>\n",ptr->row,ptr->column);

	}

}

#endif

/*-----------static-functions---declarations------------*/
/*get_head_element::return the header of the corresponding list*/
static struct queen * get_head_element(int list_id){
	if(list_id>array_size){
		PRINTF("queen: the array Id inserted don't exist\n");
		return 0;
	}
	return queen_array[list_id];
}

/*get_head_element::return the tail element of the corresponding list*/
static struct queen * get_tail_element(int list_id){
	if(list_id>array_size){
		PRINTF("queen: the array Id inserted don't exist\n");
		return 0;
	}
	return queen_array[list_id]->tail;
}

/*get_next_element::return the next element of the list */
static struct queen * get_next_element(struct queen * ptr){
	return ptr->next_queen;
}


