#ifndef PIECE_LIBRARY_H
#define PIECE_LIBRARY_H

#include <vector>
#include "../engine/piece.h"

class PieceLibrary {
public:
    static std::vector<Piece> make_all_pieces();

    static std::vector<Piece> get_piece_by_id(const std::vector<int>& ids);
};

#endif