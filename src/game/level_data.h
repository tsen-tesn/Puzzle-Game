#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <vector>
#include "../engine/piece.h"

class LevelData {
public:
    int width;
    int height;
    std::vector<Piece> pieces;
};

#endif