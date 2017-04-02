/*
 * Solver.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: orr
 */

#include "Solver.h"
#include <stdio.h>


sudoku::Solver::Solver(const Board& solvee):m_solvee(solvee),last_solution(false) {
	Board::Status s=m_solvee.simpleSolve();
	if (s==Board::SOLVED)
	{
		last_solution=true;
	}
	if (s==Board::DEAD_END)
	{
		dead_end=true;
	}
	/*boards.push(solvee);
	Board::Status s=boards.top().simpleSolve();
	if (s==Board::SOLVED)
	{
		boards.top().PrintBoard();
	}
	if (s==Board::DEAD_END)
		printf("invalid board supplied\n");*/
}
int sudoku::Solver::steps(Board & b, Board &out)
{
	Board::Status s=b.simpleSolve();
	if (s==Board::SOLVED)
	{
		b.PrintBoard();
		out=b;
		return 1;
	}
	if (s==Board::DEAD_END)
		return 0;
	for (int i=0;i<81;++i)
	{
		int possible = b.getPossible(i);
		if (bit_span[possible][BITS_ON]>=2)
		{
			
			for (int j=1;j<=bit_span[possible][BITS_ON];++j)
			{
				Board new_b=b;
				new_b.setValue(i,bit_span[possible][j]);
				printf("trying %d in cell %d\n",bit_span[possible][j],i);
				if (steps(new_b,out))
					return 1;
				printf("trying %d in cell %d led to dead end\n",bit_span[possible][j],i);
			}
			return 0;
		}
	}
	return 0;
}

void sudoku::Solver::nextSolution()
{
	steps(m_solvee,m_c_solved);
	/*
	bool pushed=false;
	if (!last_solution)
	{
		for (int i=0;i<81;++i)
		{
			int possible = boards.top().getPossible(i);
			if (bit_span[possible][0]>=2)
			{
				boards.push(boards.top());
				boards.top().setValue(i,bit_span[possible][1]);
				tries.push(1|(i<<8));
				pushed=true;
				break;
			}
		}
		if (!pushed)
		{
			if (tries.size()==0)
			{
				last_solution=true;
				return;
			}
			int cell,try1;
			cell=tries.top()>>8;
			try1=tries.top()&0xff;
			boards.pop();
			int possible = boards.top().getPossible(cell);
			if (try1<bit_span[possible][0])
			{
				++try1;
				boards.push(boards.top());
				boards.top().setValue(cell,bit_span[possible][try1]);
				tries.push(try1|(cell<<8));
			}
		}
		boards.top().simpleSolve();
	}
	*/
}

