#ifndef CELL_H
#define CELL_H

class Cell{
public:
    int x, y;

    bool operator<(const Cell& o) const {
        return y < o.y || (y == o.y && x < o.x);
    }
    bool operator==(const Cell& o) const {
        return x == o.x && y == o.y;
    }
};
#endif