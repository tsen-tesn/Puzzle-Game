#include <gtest/gtest.h>
#include <iostream>
#include "../src/board.h"

TEST(BoardTest, MakeBoardTEST) {
    Board board{5, 10};
    EXPECT_TRUE(board.in_bounds(Cell{0, 0}));
    EXPECT_TRUE(board.in_bounds(Cell{4, 9}));
    EXPECT_FALSE(board.in_bounds(Cell{5, 0}));
    EXPECT_FALSE(board.in_bounds(Cell{-1, 0}));
}

TEST(BoardTest, CanPlaceTest) {
    Board board{5, 5};
    std::vector<Cell> shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{3, 0},
        Cell{0, 1}
    };

    EXPECT_TRUE(board.can_place(shape, Cell{1, 1}));
}

TEST(BoardTest, PlaceTestWithZeroOffset) {
    Board board{5, 5};
    std::vector<Cell> shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{3, 0},
        Cell{0, 1}
    };

    EXPECT_TRUE(board.can_place(shape, Cell{0, 0}));
    board.place(0, shape, Cell{0, 0});
    EXPECT_FALSE(board.can_place(shape, Cell{0, 0}));

    std::vector<int> expected_grid = {
         0,  0,  0,  0, -1,
         0, -1, -1, -1, -1,
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1
    };
    EXPECT_EQ(expected_grid, board.get_grid());
}

TEST(BoardTest, PlaceTest) {
    Board board{5, 5};
    std::vector<Cell> shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{3, 0},
        Cell{0, 1}
    };

    EXPECT_TRUE(board.can_place(shape, Cell{1, 1}));
    board.place(0, shape, Cell{1, 1});
    EXPECT_FALSE(board.can_place(shape, Cell{1, 1}));

    std::vector<int> expected_grid = {
        -1, -1, -1, -1, -1,
        -1,  0,  0,  0,  0,
        -1,  0, -1, -1, -1,
        -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1
    };
    EXPECT_EQ(expected_grid, board.get_grid());
}

TEST(BoardTest, RemoveTest) {
    Board board{5, 5};
    std::vector<Cell> shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{3, 0},
        Cell{0, 1}
    };

    EXPECT_TRUE(board.can_place(shape, Cell{1, 1}));
    board.place(0, shape, Cell{1, 1});
    EXPECT_FALSE(board.can_place(shape, Cell{1, 1}));

    board.remove(0, shape, Cell{1, 1});
    EXPECT_TRUE(board.can_place(shape, Cell{1, 1}));

    std::vector<int> expected_grid(25, -1);
    EXPECT_EQ(expected_grid, board.get_grid());
}

TEST(BoardTest, CannotPlaceTest) {
    Board board{5, 5};
    std::vector<Cell> shape{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{3, 0},
        Cell{0, 1}
    };

    board.place(0, shape, Cell{1, 1});
    EXPECT_FALSE(board.can_place(shape, Cell{2, 1})); // Overlap
    EXPECT_FALSE(board.can_place(shape, Cell{3, 4})); // Out of bounds
}

