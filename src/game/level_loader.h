#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include "level_data.h"

class LevelLoader {
public:
    static LevelData load_level(const std::string& filename);
};

#endif