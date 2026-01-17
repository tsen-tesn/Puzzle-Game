#ifndef SOLVE_API_H
#define SOLVE_API_H
#include <vector>
#include <string>
#include "../engine/placement.h"

class SolveRequest {
public:
    int width = 0;
    int height = 0;
    std::vector<int> piece_ids; 
};

class CellDTO {
public:
    int x = 0;
    int y = 0;
};

class PlacementDTO {
public:
    int pieceId = -1;
    int variantIndex = -1;              // debug 用
    std::vector<CellDTO> cells;         // absolute cells
};

class SolveResult {
public:
    bool solved = false;
    std::vector<PlacementDTO> placements;
    std::string error_message; 
};

SolveResult solve_puzzle(const SolveRequest& req);

#endif 