#include <iostream>
// #include "./engine/board.h"
// #include "./engine/piece.h"
// #include "./engine/solver.h"
// #include "./game/game.h"
#include "./web/solve_api.h"

int main() {
    SolveRequest request;
    request.width = 4;
    request.height = 5;
    request.piece_ids = {0, 1, 2, 3};

    auto result = solve_puzzle(request);
    if (result.solved) {
        std::cout << "placements = " << result.placements.size() << "\n";
        if (!result.placements.empty()) {
            const auto& p = result.placements[0];
            std::cout << "pieceId = " << p.pieceId << ", cells = " << 5 << std::endl;
            for (const auto& p : result.placements) {   
                for (const auto& c : p.cells) {   
                    std::cout << "(" << c.x << "," << c.y << ") ";
                }
                std::cout << std::endl;
            }
        }
    } else {
        std::cout << "solved = false, error = " << result.error_message << std::endl;
    }
    return 0;
}
