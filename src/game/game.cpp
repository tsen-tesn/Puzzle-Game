#include "game.h"


Game::Game(int w, int h, std::vector<Piece> pieces) : width(w), height(h), pieces(std::move(pieces)), board(w, h){}

void Game::run() {
    while (true) {
        print_menu();
        int cmd;
        std::cin >> cmd;

        if (cmd == 1) {
            solve_game();
        } else if (cmd == 2) {
            reset_game();
        } else if (cmd == 0) {
            break;
        }
    }
}

void Game::print_menu() const {
    std::cout << "\n=== Puzzle Game ===\n";
    std::cout << "1. Solve puzzle\n";
    std::cout << "2. Reset\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose: ";
}

void Game::print_board() const {
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            std::cout << board.get_grid()[i * width + j] << " ";
        }
        std::cout << "\n";
    }
}
void Game::solve_game() {
    board.clear();
    Solver solver(board, pieces);
    
    if (solver.solve()) {
        std::cout << "Solved!\n";

        Board show(width, height);
        for (const auto& p : solver.get_placements_path()) {
            const Piece& piece = pieces[p.get_piece_id()];
            const auto& v = piece.get_variants()[p.get_variant_index()];
            show.place(p.get_piece_id(), v, p.get_offset());
        }
        show.print();
    } else {
        std::cout << "No solution.\n";
    }
}

void Game::reset_game() {
    board.clear();
    std::cout << "Game reset.\n";
}
