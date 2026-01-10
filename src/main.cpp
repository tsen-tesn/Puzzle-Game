#include <iostream>
#include "./engine/board.h"
#include "./engine/piece.h"
#include "./engine/solver.h"
#include "./game/game.h"

int main() {
    std::vector<Cell> shape1{
        Cell{0, 0}, Cell{1, 0}, Cell{2, 0}, Cell{1, 1}, Cell{1, 2}
    };

    std::vector<Cell> shape2{
        Cell{0, 0}, Cell{1, 0}, Cell{1, 1}, Cell{1, 2}, Cell{1, 3}
    };

    std::vector<Cell> shape3{
        Cell{0, 0},
        Cell{1, 0},
        Cell{1, 1},
        Cell{2, 0},
        Cell{3, 0}
    };

    std::vector<Cell> shape4{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{1, 1},
        Cell{2, 1}
    };

    std::vector<Cell> shape5{
        Cell{0, 0},
        Cell{1, 0},
        Cell{1, 1},
        Cell{2, 1},
        Cell{2, 2}
    };

    std::vector<Piece> pieces;
    pieces.emplace_back(Piece{0, shape1});
    pieces.emplace_back(Piece{1, shape2});
    pieces.emplace_back(Piece{2, shape3});
    pieces.emplace_back(Piece{3, shape4});
    pieces.emplace_back(Piece{4, shape5});

    Board board(5, 5);
    Solver solver(board, pieces);
    if (solver.solve()) {
        std::cout << "Solved!\n";
        Board show(board.get_width(), board.get_height());
        for (const auto& p : solver.get_placements_path()) {
            const Piece& piece = pieces[p.get_piece_id()];
            const auto& variant = piece.get_variants()[p.get_variant_index()];
            show.place(p.get_piece_id(), variant, p.get_offset()); } show.print();
    }else{
        std::cout << "No solution.\n";
    }

    // Run the game interface
    Game game(5, 5, pieces);
    game.run();

    return 0;
}

