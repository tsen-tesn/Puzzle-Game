#ifndef SOLVER_H
#define SOLVER_H

#include "board.h"
#include "placement.h"

class Solver {
    // Solver Steps :
    // 1. Find empty cell
    // 2. Try to place each piece variant at that cell
    // 3. If placed, recurse to step 1
    // 4. If no pieces can be placed, backtrack

private:
    bool dfs();

    Board& board;
    const std::vector<Piece>& pieces;
    std::vector<int> piece_used;    // 0 -> not used, 1 -> used
    std::vector<Placement> placements_path; // current placements

public:
    Solver(Board& b, const std::vector<Piece>& p);

    bool solve();

    const std::vector<Placement>& get_placements_path() const;
};
#endif