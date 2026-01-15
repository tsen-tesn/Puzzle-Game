#include "piece_library.h"

static std::vector<std::vector<Cell>> all_shapes() {
    return{
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 1}}, // long L shape
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {1, 1}}, // long T shape
        {{0, 0}, {1, 0}, {2, 0}, {1, 1}, {1, 2}}, // T shape
        {{0, 0}, {1, 0}, {2, 0}, {1, 1}, {2, 1}}, // G shape
        {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}}, // Ladder shape
        {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}}, // S shape
        {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {0, 2}}, // L shape
        {{1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}}, // Z shape
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}, // I shape
        {{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}}, // X shape
        {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {1, 2}}, // strange shape
        {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, 1}}  // U shape
    };
}

std::vector<Piece> PieceLibrary::make_all_pieces() {
    std::vector<Piece> pieces;
    auto shapes = all_shapes();
    for (int i = 0; i < shapes.size(); ++i) {
        pieces.emplace_back(Piece{i, shapes[i]});
    }
    return pieces;
}

std::vector<Piece> PieceLibrary::get_piece_by_id(const std::vector<int>& ids) {
    std::vector<Piece> all_pieces = make_all_pieces();
    std::vector<Piece> selected_pieces;
    for (int id : ids) {
        if (id >= 0 && id < all_pieces.size()) {
            selected_pieces.push_back(all_pieces[id]);
        }
    }
    return selected_pieces;
}