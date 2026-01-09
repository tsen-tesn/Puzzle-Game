#ifndef PEICE_H
#define PEICE_H

#include <vector>
#include <string>
#include "cell.h"

class Piece {
private:
    int id;
    std::vector<Cell> shape;
    std::vector<std::vector<Cell>> variants;
public:
    Piece(int id, std::vector<Cell> shape);

    // variants
    std::vector<Cell> apply(const std::vector<Cell>& base_shape, const int mode);
    static std::vector<Cell> normalize(const std::vector<Cell>& shape); // move to orginization and sort

    // turn and mirror
    static Cell turn90(Cell c);
    static Cell turn180(Cell c);
    static Cell turn270(Cell c);
    static Cell mirror(Cell c);

    int get_id() const;
    const std::vector<Cell>& get_shape() const;
    const std::vector<std::vector<Cell>>& get_variants() const;

};

#endif
