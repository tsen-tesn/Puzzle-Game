#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "../src/game/level_data.h"
#include "../src/game/level_loader.h"
#include "../src/game/game.h"

TEST(LoadTest, LoadSampleTest) {
    std::filesystem::path sample_path = "../levels/The small slam/levelA/level3.txt";
    std::ifstream file(sample_path);
    EXPECT_TRUE(file.is_open());
    std::string line1, line2, line3;
    std::getline(file, line1);
    std::getline(file, line2);
    std::getline(file, line3);
    EXPECT_EQ("3 5\r", line1);
    EXPECT_EQ("\r", line2);
    EXPECT_EQ("0 1 2\r", line3);
}

TEST(LoadTest, LevelDataStructureTest) {
    LevelData ld;
    ld.width = 4;
    ld.height = 6;
    EXPECT_EQ(4, ld.width);
    EXPECT_EQ(6, ld.height);
}

TEST(LoadTest, LoadLevelTest) {
    LevelData ld = LevelLoader::load_level("../levels/The small slam/levelA/level3.txt");
    EXPECT_EQ(3, ld.width);
    EXPECT_EQ(5, ld.height);
    EXPECT_EQ(3, ld.pieces.size());
}

