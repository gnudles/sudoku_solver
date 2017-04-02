/*
 * Board.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: orr
 */

#include "Board.h"
#include <stdio.h>

namespace sudoku {
void Board::PrintBoard() const
{
	for (int i=0;i<81;++i)
	{
		if(i%9==0)
			printf("\n");
		printf("%d ",bdata[i].value);
	}
	printf("\n");
}
void Board::PrintCellPossiblity(int cell) const
{
	printf("[%2d] ",cell);
	int possible=bdata[cell].possible;
	for (int i=1;i<=bit_span[possible][BITS_ON];++i)
	{
		printf("%2d ",bit_span[possible][i]);
	}
	printf("\n");
}
Board::Status Board::checkDoubles() const
{
	for (int box=0;box<9;++box)//boxes
	{
		unsigned short possible=0;
		for (int i=0;i<9;++i)
		{
			if ((possible & GETBIT(bdata[box_access[box][i]].value))!=0)
				return DEAD_END;
			possible|=GETBIT(bdata[box_access[box][i]].value);
		}
	}
	for (int row=0;row<9;++row)//rows
	{
		unsigned short possible=0;
		for (int i=0;i<9;++i)
		{
			if ((possible & GETBIT(bdata[row_access[row][i]].value))!=0)
				return DEAD_END;
			possible|=GETBIT(bdata[row_access[row][i]].value);
		}
	}
	for (int column=0;column<9;++column)//columns
	{
		unsigned short possible=0;
		for (int i=0;i<9;++i)
		{
			if ((possible & GETBIT(bdata[column_access[column][i]].value))!=0)
				return DEAD_END;
			possible|=GETBIT(bdata[column_access[column][i]].value);
		}
	}
	return NO_CHANGE;
}

Board::Status Board::checkPossible()
{
	int invalidated=0;
	int unsolved=0;
	
	//PrintBoard();
	for (int cell=0;cell<81;++cell)
	{
		if (bdata[cell].value==0)
		{
			++unsolved;
			int possible = bdata[cell].possible;
			int value;
			for (int i=0;i<CONNECTED_CELLS;++i)
			{
				value=bdata[cell_connected[cell][i]].value;
				if (value!=0)
					possible &= ~GETBIT(value);
			}
			if (possible == 0)//something went wrong...
			{
				return DEAD_END;
			}
			if (bdata[cell].possible != possible)
			{
				bdata[cell].possible = possible;
				invalidated++;
			}
			
			if (bit_span[possible][BITS_ON] == 1)// first cell is number of bits on. check if only one bit is on.
			{
				printf("cell %d [r %d,c %d] must be %d\n",cell, cell/9, cell%9, bit_span[possible][1]);
				bdata[cell].value=bit_span[possible][1];
				//invalidatePossible(cell);
				invalidated++;
			}
			
		}
		
	}
	if (invalidated>0)
		return INVALIDATED;
	if (unsolved==0)
		return SOLVED;
	return NO_CHANGE;
}

Board::Status Board::checkNoPossibility() const
{
	for (int box=0;box<9;++box)//boxes
	{
		unsigned short possible=0;
		for (int i=0;i<9;++i)
		{
			if (bdata[box_access[box][i]].value)
			{
				possible|=GETBIT(bdata[box_access[box][i]].value);
			}
			else
			{
				possible|=bdata[box_access[box][i]].possible;
			}
		}
		if (possible != NINE_BITS)
			return DEAD_END;
	}
	for (int row=0;row<9;++row)//rows
	{
		unsigned short possible=0;
		for (int i=0;i<9;++i)
		{
			if (bdata[row_access[row][i]].value)
			{
				possible|=GETBIT(bdata[row_access[row][i]].value);
			}
			else
			{
				possible|=bdata[row_access[row][i]].possible;
			}
		}
		if (possible != NINE_BITS)
			return DEAD_END;
	}
	for (int column=0;column<9;++column)//columns
	{
		unsigned short possible=0;
		for (int i=0;i<9;++i)
		{
			if (bdata[column_access[column][i]].value)
			{
				possible|=GETBIT(bdata[column_access[column][i]].value);
			}
			else
			{
				possible|=bdata[column_access[column][i]].possible;
			}
		}
		if (possible != NINE_BITS)
			return DEAD_END;
	}
	return NO_CHANGE;
}
Board::Status Board::checkPossibleUnionCollision()
{
	const int UN_INITIALIZED=-1;
	const int DIFFERENT=-2;
	int invalidated=0;
	for (int box=0;box<9;++box)//boxes
	{
		int possible=0;
		for (int i=0;i<9;++i)
		{
			int value= bdata[box_access[box][i]].value;
			if (value==0)
			{
				possible|=bdata[box_access[box][i]].possible;
			}
		}
		
		for (int v=1;v<=bit_span[possible][BITS_ON];++v)
		{
			int column=UN_INITIALIZED;
			int row=UN_INITIALIZED;
			int vdrop=bit_span[possible][v];
			int vcount=0;
			for (int i=0;i<9;++i)
			{
				if (bdata[box_access[box][i]].possible& GETBIT(vdrop))//if that possiblity appears only on certain column or row
				{
					vcount++;
					if (column==UN_INITIALIZED)
						column=cell_column[box_access[box][i]];
					else if (column!=cell_column[box_access[box][i]])
						column=DIFFERENT;
					if (row==UN_INITIALIZED)
						row=cell_row[box_access[box][i]];
					else if (row!=cell_row[box_access[box][i]])
						row=DIFFERENT;
				}
			}
			if (vcount==1)
			{
				printf("vcount=1 in box for cell %d [r %d,c %d]\n",row*9+column, row, column);
				bdata[row*9+column].value=vdrop;
				++invalidated;
				//invalidatePossible(row*9+column);
			}
			if (column!=DIFFERENT)//if that possiblity appears only on that certain column
			{
				for (int i=0;i<9;++i)
				{
					if ((i/3)!=(box/3))//where the column does not intesects the box.
					{
						int cell=column_access[column][i];
						if (bdata[cell].value==0)
						{
							if (bdata[cell].possible&GETBIT(vdrop))
							{
								++invalidated;
								bdata[cell].possible&=~GETBIT(vdrop);
								printf("eliminating from cell %d possibility %d [box->column]\n",cell,vdrop);
							}
						}
					}
				}
			}
			if (row!=DIFFERENT)//if that possiblity appears only on that certain row
			{
				for (int i=0;i<9;++i)
				{
					if ((i/3)!=(box%3))//where the row does not intesects the box.
					{
						int cell=row_access[row][i];
						if (bdata[cell].value==0)
						{
							if (bdata[cell].possible&GETBIT(vdrop))
							{
								++invalidated;
								bdata[cell].possible&=~GETBIT(vdrop);
								printf("eliminating from cell %d possibility %d [box->row]\n",cell,vdrop);
							}
						}
					}
				}
			}
		}
	}
	
	//end of boxes
	for (int row=0;row<9;++row)//rows
	{
		int possible=0;
		for (int i=0;i<9;++i)
		{
			int value= bdata[row_access[row][i]].value;
			if (value==0)
			{
				possible|=bdata[row_access[row][i]].possible;
			}
		}
		for (int v=1;v<=bit_span[possible][BITS_ON];++v)
		{
			int box=UN_INITIALIZED;
			int vdrop=bit_span[possible][v];
			int vcount=0;
			int last_cell=-1;
			for (int i=0;i<9;++i)
			{
				if (bdata[row_access[row][i]].possible& GETBIT(vdrop))
				{
					vcount++;
					last_cell=row_access[row][i];
					if (box==UN_INITIALIZED)
						box=cell_box[row_access[row][i]];
					else if (box!=cell_box[row_access[row][i]])
						box=DIFFERENT;
				}
			}
			if (vcount==1)
			{
				bdata[last_cell].value=vdrop;
				printf("vcount=1 in row for cell %d [r %d,c %d]\n",last_cell,last_cell/9,last_cell%9);
				//invalidatePossible(last_cell);
				++invalidated;
			}
			if (box!=DIFFERENT)
			{
				for (int i=0;i<9;++i)//run on the box
				{
					if (cell_row[box_access[box][i]]!=row)
					{
						int cell=box_access[box][i];
						if (bdata[cell].possible&GETBIT(vdrop))
						{
							++invalidated;
							bdata[cell].possible&=~GETBIT(vdrop);
							printf("eliminating from cell %d possibility %d [row->box]\n",cell,vdrop);
						}
					}
				}
			}
		}
	}
	
	for (int column=0;column<9;++column)//columns
	{
		int possible=0;
		for (int i=0;i<9;++i)
		{
			int value= bdata[column_access[column][i]].value;
			if (value==0)
			{
				possible|=bdata[column_access[column][i]].possible;
			}
		}
		for (int v=1;v<=bit_span[possible][BITS_ON];++v)
		{
			int box=UN_INITIALIZED;
			int vdrop=bit_span[possible][v];
			int vcount=0;
			int last_cell=-1;
			for (int i=0;i<9;++i)
			{
				if (bdata[column_access[column][i]].possible& GETBIT(vdrop))
				{
					vcount++;
					last_cell=column_access[column][i];
					if (box==UN_INITIALIZED)
						box=cell_box[column_access[column][i]];
					else if (box!=cell_box[column_access[column][i]])
						box=DIFFERENT;
				}
			}
			if (vcount==1)
			{
				printf("vcount=1 in column for cell %d [r %d,c %d]\n",last_cell,last_cell/9,last_cell%9);
				bdata[last_cell].value=vdrop;
				//invalidatePossible(last_cell);
				++invalidated;
			}
			if (box!=DIFFERENT)
			{
				for (int i=0;i<9;++i)//run on the box
				{
					if (cell_column[box_access[box][i]]!=column)
					{
						int cell=box_access[box][i];
						if (bdata[cell].possible&GETBIT(vdrop))
						{
							++invalidated;
							bdata[cell].possible&=~GETBIT(vdrop);
							printf("eliminating from cell %d possibility %d [column->box]\n",cell,vdrop);
						}
					}
				}
			}
		}
	}
	if (invalidated)
		return INVALIDATED;
	return NO_CHANGE;
}
/*
for i in range(0,9):
	for j in range(0,9):
		if (j==0):
			print("{",end="");
		print ("%2d"%(i*9+j),end="");
		if (j==8):
			print("},");
		else:
			print(",",end="");
*/
const int Board::row_access[9][9]={
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8},
	{ 9,10,11,12,13,14,15,16,17},
	{18,19,20,21,22,23,24,25,26},
	{27,28,29,30,31,32,33,34,35},
	{36,37,38,39,40,41,42,43,44},
	{45,46,47,48,49,50,51,52,53},
	{54,55,56,57,58,59,60,61,62},
	{63,64,65,66,67,68,69,70,71},
	{72,73,74,75,76,77,78,79,80}
};
/*
for i in range(0,9):
	for j in range(0,9):
		if (j==0):
			print("{",end="");
		print ("%2d"%(j*9+i),end="");
		if (j==8):
			print("},");
		else:
			print(",",end="");
*/
const int Board::column_access[9][9]={
	{ 0, 9,18,27,36,45,54,63,72},
	{ 1,10,19,28,37,46,55,64,73},
	{ 2,11,20,29,38,47,56,65,74},
	{ 3,12,21,30,39,48,57,66,75},
	{ 4,13,22,31,40,49,58,67,76},
	{ 5,14,23,32,41,50,59,68,77},
	{ 6,15,24,33,42,51,60,69,78},
	{ 7,16,25,34,43,52,61,70,79},
	{ 8,17,26,35,44,53,62,71,80}
};
/*
for i in range(0,9):
	for j in range(0,9):
		if (j==0):
			print("{",end="");
		print ("%2d"%(int(j%3)+9*int(j/3)+3*int(i%3)+27*int(i/3)),end="");
		if (j==8):
			print("},");
		else:
			print(",",end="");
*/
const int Board::box_access[9][9]={
	{ 0, 1, 2, 9,10,11,18,19,20},
	{ 3, 4, 5,12,13,14,21,22,23},
	{ 6, 7, 8,15,16,17,24,25,26},
	{27,28,29,36,37,38,45,46,47},
	{30,31,32,39,40,41,48,49,50},
	{33,34,35,42,43,44,51,52,53},
	{54,55,56,63,64,65,72,73,74},
	{57,58,59,66,67,68,75,76,77},
	{60,61,62,69,70,71,78,79,80}
};
const int Board::cell_row[81]={
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8
};
const int Board::cell_column[81]={
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7, 8
};
const int Board::cell_box[81]={
	0, 0, 0, 1, 1, 1, 2, 2, 2,
	0, 0, 0, 1, 1, 1, 2, 2, 2,
	0, 0, 0, 1, 1, 1, 2, 2, 2,
	3, 3, 3, 4, 4, 4, 5, 5, 5,
	3, 3, 3, 4, 4, 4, 5, 5, 5,
	3, 3, 3, 4, 4, 4, 5, 5, 5,
	6, 6, 6, 7, 7, 7, 8, 8, 8,
	6, 6, 6, 7, 7, 7, 8, 8, 8,
	6, 6, 6, 7, 7, 7, 8, 8, 8
};
/*
	for i in range (0,9):
		for j in range (0,9):
			print ("{",end="");
			for k in range (0,9):
				if (int(k/3)!=int(j/3)):
					print ("%2d,"%(i*9+k),end="");
			for k in range (0,9):
				if (int(k/3)!=int(i/3)):
					print ("%2d,"%(k*9+j),end="");
			for k in range (0,9):
				c=int(i/3)*3*9+int(j/3)*3+int(k%3)+int(k/3)*9;
				if (c!=i*9+j):
					print ("%2d,"%(c),end="");
			print ("},");
			*/
const int Board::cell_connected[81][20]={
	{ 3, 4, 5, 6, 7, 8,27,36,45,54,63,72, 1, 2, 9,10,11,18,19,20},
	{ 3, 4, 5, 6, 7, 8,28,37,46,55,64,73, 0, 2, 9,10,11,18,19,20},
	{ 3, 4, 5, 6, 7, 8,29,38,47,56,65,74, 0, 1, 9,10,11,18,19,20},
	{ 0, 1, 2, 6, 7, 8,30,39,48,57,66,75, 4, 5,12,13,14,21,22,23},
	{ 0, 1, 2, 6, 7, 8,31,40,49,58,67,76, 3, 5,12,13,14,21,22,23},
	{ 0, 1, 2, 6, 7, 8,32,41,50,59,68,77, 3, 4,12,13,14,21,22,23},
	{ 0, 1, 2, 3, 4, 5,33,42,51,60,69,78, 7, 8,15,16,17,24,25,26},
	{ 0, 1, 2, 3, 4, 5,34,43,52,61,70,79, 6, 8,15,16,17,24,25,26},
	{ 0, 1, 2, 3, 4, 5,35,44,53,62,71,80, 6, 7,15,16,17,24,25,26},
	{12,13,14,15,16,17,27,36,45,54,63,72, 0, 1, 2,10,11,18,19,20},
	{12,13,14,15,16,17,28,37,46,55,64,73, 0, 1, 2, 9,11,18,19,20},
	{12,13,14,15,16,17,29,38,47,56,65,74, 0, 1, 2, 9,10,18,19,20},
	{ 9,10,11,15,16,17,30,39,48,57,66,75, 3, 4, 5,13,14,21,22,23},
	{ 9,10,11,15,16,17,31,40,49,58,67,76, 3, 4, 5,12,14,21,22,23},
	{ 9,10,11,15,16,17,32,41,50,59,68,77, 3, 4, 5,12,13,21,22,23},
	{ 9,10,11,12,13,14,33,42,51,60,69,78, 6, 7, 8,16,17,24,25,26},
	{ 9,10,11,12,13,14,34,43,52,61,70,79, 6, 7, 8,15,17,24,25,26},
	{ 9,10,11,12,13,14,35,44,53,62,71,80, 6, 7, 8,15,16,24,25,26},
	{21,22,23,24,25,26,27,36,45,54,63,72, 0, 1, 2, 9,10,11,19,20},
	{21,22,23,24,25,26,28,37,46,55,64,73, 0, 1, 2, 9,10,11,18,20},
	{21,22,23,24,25,26,29,38,47,56,65,74, 0, 1, 2, 9,10,11,18,19},
	{18,19,20,24,25,26,30,39,48,57,66,75, 3, 4, 5,12,13,14,22,23},
	{18,19,20,24,25,26,31,40,49,58,67,76, 3, 4, 5,12,13,14,21,23},
	{18,19,20,24,25,26,32,41,50,59,68,77, 3, 4, 5,12,13,14,21,22},
	{18,19,20,21,22,23,33,42,51,60,69,78, 6, 7, 8,15,16,17,25,26},
	{18,19,20,21,22,23,34,43,52,61,70,79, 6, 7, 8,15,16,17,24,26},
	{18,19,20,21,22,23,35,44,53,62,71,80, 6, 7, 8,15,16,17,24,25},
	{30,31,32,33,34,35, 0, 9,18,54,63,72,28,29,36,37,38,45,46,47},
	{30,31,32,33,34,35, 1,10,19,55,64,73,27,29,36,37,38,45,46,47},
	{30,31,32,33,34,35, 2,11,20,56,65,74,27,28,36,37,38,45,46,47},
	{27,28,29,33,34,35, 3,12,21,57,66,75,31,32,39,40,41,48,49,50},
	{27,28,29,33,34,35, 4,13,22,58,67,76,30,32,39,40,41,48,49,50},
	{27,28,29,33,34,35, 5,14,23,59,68,77,30,31,39,40,41,48,49,50},
	{27,28,29,30,31,32, 6,15,24,60,69,78,34,35,42,43,44,51,52,53},
	{27,28,29,30,31,32, 7,16,25,61,70,79,33,35,42,43,44,51,52,53},
	{27,28,29,30,31,32, 8,17,26,62,71,80,33,34,42,43,44,51,52,53},
	{39,40,41,42,43,44, 0, 9,18,54,63,72,27,28,29,37,38,45,46,47},
	{39,40,41,42,43,44, 1,10,19,55,64,73,27,28,29,36,38,45,46,47},
	{39,40,41,42,43,44, 2,11,20,56,65,74,27,28,29,36,37,45,46,47},
	{36,37,38,42,43,44, 3,12,21,57,66,75,30,31,32,40,41,48,49,50},
	{36,37,38,42,43,44, 4,13,22,58,67,76,30,31,32,39,41,48,49,50},
	{36,37,38,42,43,44, 5,14,23,59,68,77,30,31,32,39,40,48,49,50},
	{36,37,38,39,40,41, 6,15,24,60,69,78,33,34,35,43,44,51,52,53},
	{36,37,38,39,40,41, 7,16,25,61,70,79,33,34,35,42,44,51,52,53},
	{36,37,38,39,40,41, 8,17,26,62,71,80,33,34,35,42,43,51,52,53},
	{48,49,50,51,52,53, 0, 9,18,54,63,72,27,28,29,36,37,38,46,47},
	{48,49,50,51,52,53, 1,10,19,55,64,73,27,28,29,36,37,38,45,47},
	{48,49,50,51,52,53, 2,11,20,56,65,74,27,28,29,36,37,38,45,46},
	{45,46,47,51,52,53, 3,12,21,57,66,75,30,31,32,39,40,41,49,50},
	{45,46,47,51,52,53, 4,13,22,58,67,76,30,31,32,39,40,41,48,50},
	{45,46,47,51,52,53, 5,14,23,59,68,77,30,31,32,39,40,41,48,49},
	{45,46,47,48,49,50, 6,15,24,60,69,78,33,34,35,42,43,44,52,53},
	{45,46,47,48,49,50, 7,16,25,61,70,79,33,34,35,42,43,44,51,53},
	{45,46,47,48,49,50, 8,17,26,62,71,80,33,34,35,42,43,44,51,52},
	{57,58,59,60,61,62, 0, 9,18,27,36,45,55,56,63,64,65,72,73,74},
	{57,58,59,60,61,62, 1,10,19,28,37,46,54,56,63,64,65,72,73,74},
	{57,58,59,60,61,62, 2,11,20,29,38,47,54,55,63,64,65,72,73,74},
	{54,55,56,60,61,62, 3,12,21,30,39,48,58,59,66,67,68,75,76,77},
	{54,55,56,60,61,62, 4,13,22,31,40,49,57,59,66,67,68,75,76,77},
	{54,55,56,60,61,62, 5,14,23,32,41,50,57,58,66,67,68,75,76,77},
	{54,55,56,57,58,59, 6,15,24,33,42,51,61,62,69,70,71,78,79,80},
	{54,55,56,57,58,59, 7,16,25,34,43,52,60,62,69,70,71,78,79,80},
	{54,55,56,57,58,59, 8,17,26,35,44,53,60,61,69,70,71,78,79,80},
	{66,67,68,69,70,71, 0, 9,18,27,36,45,54,55,56,64,65,72,73,74},
	{66,67,68,69,70,71, 1,10,19,28,37,46,54,55,56,63,65,72,73,74},
	{66,67,68,69,70,71, 2,11,20,29,38,47,54,55,56,63,64,72,73,74},
	{63,64,65,69,70,71, 3,12,21,30,39,48,57,58,59,67,68,75,76,77},
	{63,64,65,69,70,71, 4,13,22,31,40,49,57,58,59,66,68,75,76,77},
	{63,64,65,69,70,71, 5,14,23,32,41,50,57,58,59,66,67,75,76,77},
	{63,64,65,66,67,68, 6,15,24,33,42,51,60,61,62,70,71,78,79,80},
	{63,64,65,66,67,68, 7,16,25,34,43,52,60,61,62,69,71,78,79,80},
	{63,64,65,66,67,68, 8,17,26,35,44,53,60,61,62,69,70,78,79,80},
	{75,76,77,78,79,80, 0, 9,18,27,36,45,54,55,56,63,64,65,73,74},
	{75,76,77,78,79,80, 1,10,19,28,37,46,54,55,56,63,64,65,72,74},
	{75,76,77,78,79,80, 2,11,20,29,38,47,54,55,56,63,64,65,72,73},
	{72,73,74,78,79,80, 3,12,21,30,39,48,57,58,59,66,67,68,76,77},
	{72,73,74,78,79,80, 4,13,22,31,40,49,57,58,59,66,67,68,75,77},
	{72,73,74,78,79,80, 5,14,23,32,41,50,57,58,59,66,67,68,75,76},
	{72,73,74,75,76,77, 6,15,24,33,42,51,60,61,62,69,70,71,79,80},
	{72,73,74,75,76,77, 7,16,25,34,43,52,60,61,62,69,70,71,78,80},
	{72,73,74,75,76,77, 8,17,26,35,44,53,60,61,62,69,70,71,78,79}};
} /* namespace sudoku */
