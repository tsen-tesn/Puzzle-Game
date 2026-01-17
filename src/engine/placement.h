#ifndef PLACEMENT
#define PLACEMENT

#include "cell.h"

class Placement {
private:
    int piece_id;
    int variant_index;
    Cell offset;

public:
    Placement(int id, int var_id, const Cell& off) : piece_id(id), variant_index(var_id), offset(off) {}
    int get_piece_id() const { return piece_id; }
    int get_variant_index() const { return variant_index; }
    const Cell& get_offset() const { return offset; }
};

#endif


// placement.h
// --------------------------------
// Represents a single placement decision in a solution:
// - which piece
// - which variant
// - placed at which offset
//
// This is used as:
// - Solver output
// - API output
// - Step-by-step replay
//
// Placement is a pure data object.
