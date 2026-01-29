#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <vector>
#include "../engine/piece.h"

struct LevelInfo {
    std::string id;          
    std::string name;
    int width;
    int height;
    std::vector<int> pieceIds;
};

class LevelData {
public:
    int width;
    int height;
    std::vector<Piece> pieces;
};

#endif