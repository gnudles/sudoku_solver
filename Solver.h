/*
 * Solver.h
 *
 *  Created on: Apr 30, 2016
 *      Author: orr
 */

#ifndef SOLVER_H_
#define SOLVER_H_
#include "Board.h"
#include <stack>
using namespace std;
namespace sudoku {
extern char bit_span[512][10];
class Solver {
public:
	Solver(const Board& solvee);
	~Solver(){};
    void nextSolution();
    static int steps(Board &b, Board & out);
	const Board& solved(){return m_c_solved;}
private:
    stack<int> tries;
    stack<Board> boards;
    Board m_solvee;
	Board m_c_solved;
    bool last_solution;
	bool dead_end;
};

} /* namespace sudoku */

#endif /* SOLVER_H_ */
