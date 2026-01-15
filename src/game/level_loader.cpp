#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include "level_loader.h"
#include "piece_library.h"

LevelData LevelLoader::load_level(const std::string& filename) {
    LevelData levelData;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open level file: " + filename);
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        if(!(iss >> levelData.width >> levelData.height)) {
            throw std::runtime_error("Bad first line (expected: W H): " + line);
        }
        break;
    }

    std::vector<int> ids;
    std::getline(file, line); // skip empty line
    size_t position = 0;
    std::getline(file, line); // the piece lines
    
    while(true){
        size_t next = line.find(' ', position);
        if(next == std::string::npos){
            ids.emplace_back(std::stoi(line.substr(position)));
            break;
        }
        ids.emplace_back(std::stoi(line.substr(position, next - position)));
        position = next + 1;
    }
    std::cout << "test ids size is: " << ids.size() << std::endl;
    levelData.pieces = PieceLibrary::get_piece_by_id(ids);
    return levelData;
}
