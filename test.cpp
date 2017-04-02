#include <stdio.h>
#include "Board.h"
#include "Solver.h"
short board1[81]={
	0, 8, 1, 0, 0, 9, 0, 5, 0,
	0, 0, 0, 0, 0, 0, 0, 6, 0,
	4, 7, 0, 0, 5, 0, 2, 0, 0,
	0, 0, 0, 0, 0, 5, 6, 0, 3,
	0, 0, 4, 0, 0, 0, 0, 0, 0,
	7, 0, 3, 4, 0, 0, 0, 0, 0,
	0, 0, 5, 0, 1, 0, 0, 9, 6,
	0, 4, 0, 0, 0, 0, 0, 0, 0,
	0, 3, 0, 7, 0, 0, 4, 1, 0
};
short board2[81]={
	4, 0, 2, 3, 7, 5, 8, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 8, 0, 0, 4, 0, 0, 3, 0,
	0, 0, 8, 0, 6, 0, 2, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 5,
	0, 0, 3, 4, 0, 9, 7, 0, 0,
	8, 1, 0, 0, 0, 0, 0, 6, 2,
	0, 0, 0, 2, 3, 8, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0
};
void solve_test(short *board)
{
	sudoku::Board b(board);
	printf("got board:\n");
	b.PrintBoard();

	sudoku::Board::Status stat = b.simpleSolve();
	if (stat == sudoku::Board::SOLVED)
	{
		printf("Simple Solution\n");
	}
	else if (stat == sudoku::Board::DEAD_END)
	{
		printf("No Solution\n");
	}
	else
	{
		printf("Trying recursive solving...\n");
	}
	
    sudoku::Solver s(b);
    s.nextSolution();
	const sudoku::Board& resb=s.solved();
	if (resb.getValue(0)!=0)
	{
		printf("Found Solution\n");
		resb.PrintBoard();
	}
	else
	{
		printf("No Solution\n");
	}
}

int main()
{
	solve_test(board1);
	printf("XXXXXXXXXXXXXXXXXXXXXXXXXX\n");
	solve_test(board2);
	return 0;
}
