#include "Chess_Board.h"

#define TRUE 0
#define FALSE 0


Chess_Board::Chess_Board(int Size){
	int Index;
	Board = (int *) calloc (Size, sizeof *Board);
	Row_Control=(bool *) calloc (Size, sizeof *Row_Control);
	Col_Control=(bool *) calloc (Size, sizeof *Col_Control);
	Diag_Control=(bool *) calloc (2 * Size-1, sizeof *Diag_Control);
	AntiD_Control=(bool *) calloc (2* Size-1, sizeof *AntiD_Control);
	size=Size;
	results=0;

	if(!Board || !Row_Control || !Col_Control || !Diag_Control || !AntiD_Control){
		std::cout<<"error calloc failed --out of memory";
		exit(6);
	}

}

Chess_Board::Chess_Board(volatile Chess_Board &obj): results(obj.results),size(obj.size), Board(new int[obj.size]), Row_Control(new bool[obj.size]),Col_Control(new bool[obj.size]),Diag_Control(new bool[2*obj.size-1]),AntiD_Control(new bool[2*obj.size-1])
{
	std::copy(obj.Board,obj.Board+ obj.size, Board);
	std::copy(obj.Row_Control,obj.Row_Control + obj.size, Row_Control);
	std::copy(obj.Col_Control,obj.Col_Control + obj.size, Col_Control);
	std::copy(obj.Diag_Control,obj.Diag_Control + 2*obj.size-1, Diag_Control);
	std::copy(obj.AntiD_Control,obj.AntiD_Control + 2*obj.size-1, AntiD_Control);	
}

char Chess_Board::Check_Solution(int row){
	return !(Diag_Control[row-Board[row]+size-1] || AntiD_Control[row+Board[row]]);
}

char Chess_Board::Valid(int row){
	int Idx;        // Index into Diag[] / AntiD[]
	int Chk;        // Occupied flag

	// Diagonal:  Row-Col == constant
	   Idx = row - Board[row] + size-1;
	   Chk = Diag_Control[Idx];
	// AntiDiagonal:  Row+Col == constant
	   Idx = row + Board[row];
	    Chk = Chk || AntiD_Control[Idx];
	    return !Chk;    // Valid if NOT any occupied

}

char Chess_Board::Valid_Op (int Row, int Col)
{
   int Idx;        /* Index into Diag[] / AntiD[] */
   int Chk;        /* Occupied flag               */
				   /*Board[Row] return the current column*/
					/*Row CURRENT ROW*/
   Chk = Col_Control[Col] || Row_Control[Row];
/* Diagonal:  Row-Col == constant */
   Idx = Row - Col + size-1;
   Chk = Chk || Diag_Control[Idx];
/* AntiDiagonal:  Row+Col == constant */
   Idx = Row + Col;
   Chk = Chk || AntiD_Control[Idx];
   return !Chk;    /* Valid if NOT any occupied   */
}


void Chess_Board::mark_Solution(int row,int column, bool flag){

	Diag_Control[row-column+size-1]=flag; AntiD_Control[row+column]=flag;
	Col_Control[column]=flag;Row_Control[row]=flag;
	if(flag){
		Board[column]=row;
	}else{
		Board[column]=0;
	}

}


bool Chess_Board::complete( ){
	int i;
	for (i=0;i < size; i++){
		if (Row_Control[i]!=true)
			return false;
	}
	return true;
}

void Chess_Board::PrintBoard (void)
{
	int i;
	std::cout<<"[ \n";
	for(i=0;i<size;i++){
		for(int j=0; j<size; j++){
			if(Board[i]==j){
				std::cout<<"Q";
			}else{
				std::cout<<".";
			}
		}
		 std::cout<< "\n";
	}
	std::cout<<"]\n";


	std::cout<<"[ \n";
		for(i=size-1;i>=0;i--){
			for(int j=0; j<size; j++){
				if(Board[i]==j){
					std::cout<<"Q";
				}else{
					std::cout<<".";
				}
			}
			 std::cout<< "\n";
		}
		std::cout<<"]\n";


}


void Chess_Board::PrintVector (int type,int solv)
{	if(type==0){
		std::cout<<"( ";
		for(int i=0; i<size; i++){
			std::cout<<Board[i]<<" ";
		}
		std::cout<<") \n";
			if(solv){
				std::cout<<"( ";
					for(int i=size-1; i>=0; i--){
						std::cout<<Board[i]<<" ";
					}
					std::cout<<") \n";
			}
	}else if(type==1){
		std::cout<<"( ";
				for(int i=0; i<size; i++){
					std::cout<<Row_Control[i]<<" ";
				}
				std::cout<<") \n";
	}else if(type==2){
		std::cout<<"( ";
				for(int i=0; i<size; i++){
					std::cout<<Col_Control[i]<<" ";
				}
				std::cout<<") \n";
	}else if(type==3){
		std::cout<<"( ";
				for(int i=0; i<(2 * size-1); i++){
					std::cout<<Diag_Control[i]<<" ";
				}
				std::cout<<") \n";
	}else if(type==4){
		std::cout<<"( ";
				for(int i=0; i<(2 * size-1); i++){
					std::cout<<AntiD_Control[i]<<" ";
				}
				std::cout<<") \n";
	}
}
