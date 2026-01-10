#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include "piece.h"

class Board {
private:
    int weight, height;
    std::vector<int> grid;

public:
    Board(int w, int h);

    bool in_bounds(const Cell& p) const;
    bool is_empty(const Cell& p) const;

    bool can_place(const std::vector<Cell>& variant, const Cell& offset) const;
    void place(int piece_id, const std::vector<Cell>& variant, const Cell& offset);
    void remove(int piece_id, const std::vector<Cell>& variant, const Cell& offset);

    const std::vector<int>& get_grid() const { return grid; } // For testing purposes

    int get_width() const { return weight; }
    int get_height() const { return height; }

    void print() const; // For debugging purposes

    void clear();
};

#endif