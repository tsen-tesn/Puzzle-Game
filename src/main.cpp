#include <iostream>
#include "./engine/board.h"
#include "./engine/piece.h"
#include "./engine/solver.h"
#include "./game/game.h"

int main() {
    std::vector<Piece> empty;
    Game game(1, 1, empty);

    game.load_level("../levels/The small slam/levelA/level7.txt");
    game.print_level_contents(); 

    game.run();

    return 0;
}

