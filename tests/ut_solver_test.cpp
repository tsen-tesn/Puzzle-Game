#include <gtest/gtest.h>
#include "../src/board.h"
#include "../src/placement.h"
#include "../src/solver.h"

TEST(PlacementTest, MakePlacementTest) {
    Cell offset{2, 3};
    Placement placement{1, 0, offset};

    EXPECT_EQ(1, placement.get_piece_id());
    EXPECT_EQ(0, placement.get_variant_index());
    EXPECT_EQ(offset, placement.get_offset());
}

TEST(SolverTest, SimpleSolveTest) {
    Board board{5, 5};
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
    EXPECT_TRUE(
        board.can_place(
            variants[p.get_variant_index()],
            p.get_offset()
        )
    );
}
