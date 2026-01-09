#include "board.h"

Board::Board(int w, int h) : weight(w), height(h), grid(w * h, -1) {}

bool Board::in_bounds(const Cell& p) const {
    return p.x >=0 && p.x < weight && p.y >=0 && p.y <= height;
}

bool Board::is_empty(const Cell& p) const {
    return grid[p.y * weight + p.x] == -1;
}

bool Board::can_place(const std::vector<Cell>& variant, const Cell& offset) const {
    for (const auto& c : variant) {
            Cell p{c.x + offset.x, c.y + offset.y};
            if (!in_bounds(p) || !is_empty(p))
                return false;
        }
    return true;
}

void Board::place(int piece_id, const std::vector<Cell>& variant, const Cell& offset) {
    for (const auto& c : variant) {
        Cell p{c.x + offset.x, c.y + offset.y};
        grid[p.y * weight + p.x] = piece_id;
    }
}

void Board::remove(int piece_id, const std::vector<Cell>& variant, const Cell& offset) {
    for (const auto& c : variant) {
        Cell p{c.x + offset.x, c.y + offset.y};
        grid[p.y * weight + p.x] = -1;
    }
}