#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include "../engine/solver.h"
#include "level_loader.h"

class Game {
public:
    Game(int w, int h, std::vector<Piece> pieces);

    void debug_solve(){
        solve_game();
    }
    void run();

    void load_level(const std::string& filename);

private:
    void print_menu() const;
    void print_board() const;
    void solve_game();
    void reset_game();

    int width, height;
    std::vector<Piece> pieces;
    
    Board board;
};

#endif
