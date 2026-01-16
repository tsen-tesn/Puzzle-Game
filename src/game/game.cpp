#include "game.h"
#include <format>

Game::Game(int w, int h, std::vector<Piece> pieces) : width(w), height(h), pieces(std::move(pieces)), board(w, h){}

void Game::run() {
    while (true) {
        print_menu();
        int cmd;
        std::cin >> cmd;

        if(cmd == 1) {
            print_level_summary();
            int level_choice;
            std::cout << "Select level (0 to exit): ";
            std::cin >> level_choice;

            std::string filename;
            std::cout << "Enter level filename: ";
            std::cin >> filename;
            load_level(std::format("../levels/{0}/level{1}/level{2}.txt", level_names[level_choice] ,filename[0], filename[1]));
        } else if (cmd == 2) {
            solve_game();
        } else if (cmd == 3) {
            reset_game();
        } else if (cmd == 0) {
            break;
        }
    }
}

void Game::print_menu() const {
    std::cout << std::endl << "=== Puzzle Game ===" << std::endl;
    std::cout << "1. Choose level" << std::endl;
    std::cout << "2. Solve puzzle" << std::endl;
    std::cout << "3. Reset" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Choose: ";
}

void Game::print_level_summary() const {
    std::cout << std::endl << "=== Level Summary ===" << std::endl;
    std::cout << "1. The small slam" << std::endl;
    std::cout << "2. The slam1" << std::endl;
    std::cout << "3. The slam2" << std::endl;
    std::cout << "4. The slam3" << std::endl;
    std::cout << "5. The slam4" << std::endl;
    std::cout << "6. The grand slam" << std::endl;
    std::cout << "7. The super slam1" << std::endl;
    std::cout << "8. The super slam2" << std::endl;
    std::cout << "9. The super slam3" << std::endl;
    std::cout << "10. The super slam4" << std::endl;
    std::cout << "0. Exit" << std::endl;
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

void Game::load_level(const std::string& filename) {
    LevelData level_data = LevelLoader::load_level(filename);
    width = level_data.width;
    height = level_data.height;
    pieces = level_data.pieces;
    board = Board(width, height);

    std::cout << "Loaded level: " << filename
              << " (" << width << "x" << height
              << "), pieces=" << pieces.size() << std::endl;
}

void Game::print_level_contents() const {
    std::cout << "-----Level Summary-----" << std::endl;
    std::cout << "Board Size: " << width << " x " << height << std::endl;
    std::cout << "Pieces Count: " << pieces.size() << std::endl;
    std::cout << std::endl;

    for(const auto& piece: pieces) {
        std::cout << "Piece ID: " << piece.get_id() << std::endl;
        std::cout << "Shape: ";
        for(const auto& cell : piece.get_shape()) {
            std::cout << "(" << cell.x << ", " << cell.y << ") ";
        }
        std::cout << std::endl;
    }
}