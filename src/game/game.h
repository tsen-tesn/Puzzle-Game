#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <map>

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

    void print_level_contents() const;

private:
    void print_menu() const;
    void print_level_summary() const;
    void print_board() const;
    void solve_game();
    void reset_game();

    int width, height;
    std::vector<Piece> pieces;
    
    Board board;

    std::map<int, std::string> level_names {
        {1, "The small slam"},
        {2, "The slam1"},
        {3, "The slam2"},
        {4, "The slam3"},
        {5, "The slam4"},
        {6, "The grand slam"},
        {7, "The super slam1"},
        {8, "The super slam2"},
        {9, "The super slam3"},
        {10, "The super slam4"}
    };
};

#endif
