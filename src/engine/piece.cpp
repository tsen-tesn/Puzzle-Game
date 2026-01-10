#include <algorithm>
#include <set>
#include <iostream>
#include "piece.h"

Piece::Piece(int _id, std::vector<Cell> _shape) : id(_id), shape(_shape) {
                // variants steps :  initial shape 
                // -> turn to get the 8 kind of shape type
                // -> move to the orgin point (0,0)
                // -> sort the cell
                // -> push back the shape to variants if it has not dispeared yet
    std::set<std::vector<Cell>> seen;

    for (int i = 0; i < 8; i++) {
        auto v = apply(shape, i);
        if (seen.insert(v).second) {
            variants.push_back(v);
        }
    }
}

std::vector<Cell> Piece::apply(const std::vector<Cell>& base_shape, const int mode){
    std::vector<Cell> reg;
    for(auto c : base_shape) {
        switch(mode) {
            case 0 : reg.push_back(c); break;
            case 1 : reg.push_back(turn90(c)); break;
            case 2 : reg.push_back(turn180(c)); break;
            case 3 : reg.push_back(turn270(c)); break;
            case 4 : reg.push_back(mirror(c)); break;
            case 5 : reg.push_back(turn90(mirror(c))); break;
            case 6 : reg.push_back(turn180(mirror(c))); break;
            case 7 : reg.push_back(turn270(mirror(c))); break;
        }
    }

    return normalize(reg);
}

std::vector<Cell> Piece::normalize(const std::vector<Cell>& shape) {
    int min_x = shape[0].x, min_y = shape[0].y;
    for(auto& c : shape) {
        min_x = std::min(min_x, c.x);
        min_y = std::min(min_y, c.y);
    }

    std::vector<Cell> reg;
    for(auto& c : shape) {
        reg.push_back(Cell{c.x - min_x, c.y - min_y});
    }
    std::sort(reg.begin(), reg.end());  // The class Cell has the operation '<'
    return reg;

}

Cell Piece::turn90(Cell c) {return Cell{-c.y, c.x};}
Cell Piece::turn180(Cell c) {return Cell{-c.x, -c.y};}
Cell Piece::turn270(Cell c) {return Cell{c.y, -c.x};}
Cell Piece::mirror(Cell c) {return Cell{-c.x, c.y};}

int Piece::get_id() const {
    return id;
}
    
const std::vector<Cell>& Piece::get_shape() const{
    return shape;
}
const std::vector<std::vector<Cell>>& Piece::get_variants() const {
    return variants;
}

void Piece::print() const {
    std::cout << "Piece ID: " << id << "\n";
    std::cout << "Base Shape:\n";
    for (const auto& c : shape) {
        std::cout << "(" << c.x << ", " << c.y << ") ";
    }
    std::cout << std::endl;
}
    