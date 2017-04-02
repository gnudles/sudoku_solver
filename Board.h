/*
 * Board.h
 *
 *  Created on: Apr 30, 2016
 *      Author: orr
 */

#ifndef BOARD_H_
#define BOARD_H_

namespace sudoku {
extern char bit_span[512][10];
const int GRID_SIZE=9;
const int BITS_ON=0;
const int CONNECTED_CELLS=20;//(GRID_SIZE-sqrt(GRID_SIZE))*2+GRID_SIZE-1
const int NINE_BITS=0x1ff;
#define GETBIT(X) (1<<((X)-1))
class Board {
public:
	Board(short *board)
	{
		for (int i=0;i<81;++i)
		{
			bdata[i].value=board[i];
			if (board[i]==0)
			{
				bdata[i].given=0;
				bdata[i].possible=NINE_BITS;
			}
			else
			{
				bdata[i].given=1;
				bdata[i].possible=GETBIT(board[i]);
			}
		}
	}
	Board()
	{
		for (int i=0;i<81;++i)
		{
			bdata[i].value=0;
			bdata[i].given=0;
			bdata[i].possible=NINE_BITS;
		}
	}
	
	~Board(){};
	
	
	/*inline void invalidatePossible(int cell)
	{
		int valuep=0;
		if (bdata[cell].value!=0)
			valuep=GETBIT(bdata[cell].value);
		for (int i=0;i<CONNECTED_CELLS;++i)
		{
			bdata[cell_connected[cell][i]].possible&=~valuep;
			bdata[cell_connected[cell][i]].pvalid=0;
		}
	}*/
	int getPossible(int cell) const
	{
		if (bdata[cell].value>0)
			return GETBIT(bdata[cell].value);
		return bdata[cell].possible;
	}
	void setValue(int cell,int v)
	{
		bdata[cell].value=v;
	}
	static inline int row_column_cell(int row,int column)
	{
		return row*9+column;
	}
	
	int getValue(int cell) const
	{
		return bdata[cell].value;
	}
	
	enum Status{
		DEAD_END,
		INVALIDATED,
		NO_CHANGE,
		SOLVED
	};
	Status simpleSolve()
	{
		Status current;
		current = checkDoubles();
		if (current == DEAD_END)
			return current;
		current=INVALIDATED;
		int invalidated=0;
		while(current==INVALIDATED)
		{
			while(current==INVALIDATED)
			{
				current=checkPossible();
				if (current!=NO_CHANGE)
					++invalidated;
				if (current==SOLVED || current==DEAD_END)
					return current;
				if (checkNoPossibility() == DEAD_END)
					return DEAD_END;
			}
			current=checkPossibleUnionCollision();
			if (current!=NO_CHANGE)
				++invalidated;
		}
		if (invalidated>0)
			return INVALIDATED;
		return NO_CHANGE;
	}
	Status checkPossible();
	Status checkNoPossibility() const;
	Status checkDoubles() const;
	void PrintBoard() const;
	void PrintCellPossiblity(int cell) const;
	Status checkPossibleUnionCollision();
private:
	
	struct celldata
	{
		unsigned short given:1;
		unsigned short value:4;
		unsigned short possible:9;
		unsigned short spare:2;
		
	} bdata[81];
public:
	static const int row_access[9][9];
	static const int column_access[9][9];
	static const int box_access[9][9];
	static const int cell_row[81];
	static const int cell_column[81];
	static const int cell_box[81];
	static const int cell_connected[81][CONNECTED_CELLS];
	
};
} /* namespace sudoku */

#endif /* BOARD_H_ */
