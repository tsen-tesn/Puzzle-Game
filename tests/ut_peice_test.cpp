#include <gtest/gtest.h>
#include <algorithm>
#include <iostream>
#include "../src/cell.h"
#include "../src/piece.h"

TEST(CellTest, MakeCellTest) {
    Cell cell{1, 0};
    EXPECT_EQ(1, cell.x);
    EXPECT_EQ(0, cell.y);
}

TEST(CellTest, OperatorLessThanTest) {
    Cell c1{2, 3};
    Cell c2{3, 1};
    EXPECT_TRUE(c2 < c1);
}

TEST(CellTest, OperatorEqualTest) {
    Cell c1{2, 3};
    Cell c2{2, 3};
    EXPECT_TRUE(c1 == c2);
}

TEST(PieceTest, MakePieceTest) {
    std::vector<Cell> shape = {
        Cell{1, 1},
        Cell{1, 2},
        Cell{1, 3}
    };

    Piece piece{0, shape};
    EXPECT_EQ(0, piece.get_id());
    EXPECT_EQ(shape, piece.get_shape());
}

TEST(PieceTest, CellTurn90Test) {
    Cell c{2, 3};
    EXPECT_EQ(-3, Piece::turn90(c).x);
    EXPECT_EQ(2, Piece::turn90(c).y);
}

TEST(PieceTest, CellTurn180Test) {
    Cell c{2, 3};
    EXPECT_EQ(-2, Piece::turn180(c).x);
    EXPECT_EQ(-3, Piece::turn180(c).y);
}

TEST(PieceTest, CellTurn270Test) {
    Cell c{2, 3};
    EXPECT_EQ(3, Piece::turn270(c).x);
    EXPECT_EQ(-2, Piece::turn270(c).y);
}

TEST(PieceTest, MirrorTest) {
    Cell c{2, 3};
    EXPECT_EQ(-2, Piece::mirror(c).x);
    EXPECT_EQ(3, Piece::mirror(c).y);
}

TEST(PieceTest, MirrorAndTurn90Test) {
    Cell c{2, 3};
    EXPECT_EQ(-3, Piece::turn90(Piece::mirror(c)).x);
    EXPECT_EQ(-2, Piece::turn90(Piece::mirror(c)).y);
}

TEST(PieceTest, SortCellTest) {
    std::vector<Cell> shape{
        Cell{2, 0},
        Cell{1, 5},
        Cell{2, 3}
    };

    std::sort(shape.begin(), shape.end());
    EXPECT_EQ(0, shape[0].y);
    EXPECT_EQ(3, shape[1].y);
    EXPECT_EQ(5, shape[2].y);
}

TEST(PieceTest, NormalizeShapeTest) {
    std::vector<Cell> shape{
        Cell{2, 1},
        Cell{1, 5},
        Cell{2, 3}
    };

    std::vector<Cell> nor_shape = Piece::normalize(shape);
    Cell c1{1, 0}, c2{1, 2}, c3{0, 4};
    EXPECT_TRUE(c1 == nor_shape[0]);
    EXPECT_TRUE(c2 == nor_shape[1]);
    EXPECT_TRUE(c3 == nor_shape[2]);
}

TEST(PieceTest, GetVariantsTest) {
    std::vector<Cell> shape{
        Cell{0, 0},
        Cell{0, 1},
        Cell{0, 2},
        Cell{0, 3},
        Cell{0, 4}
    };
    Piece piece(0, shape);

    std::vector<Cell> v1{
        Cell{0, 0},
        Cell{0, 1},
        Cell{0, 2},
        Cell{0, 3},
        Cell{0, 4}
    };

    std::vector<Cell> v2{
        Cell{0, 0},
        Cell{1, 0},
        Cell{2, 0},
        Cell{3, 0},
        Cell{4, 0}
    };

    
    std::vector<std::vector<Cell>> _variants =  piece.get_variants();
    EXPECT_EQ(v1, _variants[0]);
    EXPECT_EQ(v2, _variants[1]);
}