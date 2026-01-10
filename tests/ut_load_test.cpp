#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

TEST(LoadTest, LoadSampleTest) {
    std::filesystem::path sample_path = "../levels/intermediate/levelA/level3.txt";
    std::ifstream file(sample_path);
    EXPECT_TRUE(file.is_open());
    std::string line1, line2, line3;
    std::getline(file, line1);
    std::getline(file, line2);
    std::getline(file, line3);
    EXPECT_EQ("3 5\r", line1);
    EXPECT_EQ("\r", line2);
    EXPECT_EQ("piece 0\r", line3);
}