#ifndef SOLVE_API_H
#define SOLVE_API_H
#include <vector>
#include <string>
#include "../engine/placement.h"

struct SolveRequest {
    int width = 0;
    int height = 0;
    std::vector<int> piece_ids; 
};

struct SolveResult {
    bool solved = false;
    std::vector<Placement> placements;
    std::string error_message; 
};

SolveResult solve_puzzle(const SolveRequest& req);

#endif