#include <gtest/gtest.h>
#include <algorithm>
#include "../src/engine/board.h"
#include "../src/engine/placement.h"
#include "../src/engine/solver.h"

TEST(PlacementTest, MakePlacementTest) {
    Cell offset{2, 3};
    Placement placement{1, 0, offset};

    EXPECT_EQ(1, placement.get_piece_id());
    EXPECT_EQ(0, placement.get_variant_index());
    EXPECT_EQ(offset, placement.get_offset());
}

TEST(SolverTest, SimpleSolveTest) {
    Board board{2, 2};
    std::vector<Piece> pieces;

    std::vector<Cell> square_shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{0, 1},
        Cell{1, 1}
    };
    pieces.emplace_back(Piece{0, square_shape});

    Solver solver(board, pieces);
    EXPECT_TRUE(solver.solve());

    const auto& placements = solver.get_placements_path();
    ASSERT_EQ(1, placements.size());

    const auto& p = placements[0];

    // piece id 一定是 0
    EXPECT_EQ(0, p.get_piece_id());

    // variant index 不檢查「數值」，只檢查合法
    const auto& variants = pieces[0].get_variants();
    ASSERT_LT(p.get_variant_index(), variants.size());

    // offset 必須能合法放在棋盤上
    Board empty_board{2, 2};
    EXPECT_TRUE(
        empty_board.can_place(
            variants[p.get_variant_index()],
            p.get_offset()
        )
    );
}

TEST(SolverTest, NoSolutionTest) {
    Board board{2, 2};
    std::vector<Piece> pieces;

    std::vector<Cell> line_shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0}
    };
    pieces.emplace_back(Piece{0, line_shape});

    Solver solver(board, pieces);
    EXPECT_FALSE(solver.solve());

    const auto& placements = solver.get_placements_path();
    EXPECT_EQ(0, placements.size());
}

TEST(SolverTest, MultiplePiecesTest) {
    Board board{3, 2};
    std::vector<Piece> pieces;

    std::vector<Cell> line_shape{
        Cell{0, 0},
        Cell{1, 0}
    };
    pieces.emplace_back(Piece{0, line_shape});
    pieces.emplace_back(Piece{1, line_shape});
    pieces.emplace_back(Piece{2, line_shape});

    Solver solver(board, pieces);
    EXPECT_TRUE(solver.solve());

    const auto& placements = solver.get_placements_path();
    ASSERT_EQ(3, placements.size());

    Board empty_board{3, 2};
    std::vector<int> used_piece_ids;
    for (const auto& p : placements) {
        used_piece_ids.push_back(p.get_piece_id());
    }
    EXPECT_EQ(3, used_piece_ids.size());
    std::sort(used_piece_ids.begin(), used_piece_ids.end());
    EXPECT_EQ(std::vector<int>({0, 1, 2}), used_piece_ids);

}
