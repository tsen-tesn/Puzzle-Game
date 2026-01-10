cmake_minimum_required(VERSION 3.16)
project(sample_code CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

set(GOOGLETEST_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/googletest-src")
if(EXISTS "${GOOGLETEST_SOURCE_DIR}" AND IS_DIRECTORY "${GOOGLETEST_SOURCE_DIR}")
    message(STATUS "Google Test already exists at ${GOOGLETEST_SOURCE_DIR}, skipping download")
    set(FETCHCONTENT_FULLY_DISCONNECTED ON)
else()
    message(STATUS "Google Test not found, will download to ${GOOGLETEST_SOURCE_DIR}")
    set(FETCHCONTENT_FULLY_DISCONNECTED OFF)
endif()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# -----------------------------
# Sources (engine + game)
# -----------------------------
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/engine/*.cpp)
file(GLOB_RECURSE GAME_SOURCES   CONFIGURE_DEPENDS src/game/*.cpp)

set(SOURCES ${ENGINE_SOURCES} ${GAME_SOURCES})

# -----------------------------
# App executable (main program)
# -----------------------------
add_executable(app src/main.cpp ${SOURCES})

# -----------------------------
# Unit tests
# -----------------------------
file(GLOB_RECURSE UNIT_TESTS CONFIGURE_DEPENDS tests/*.cpp)

add_executable(unit_tests ${UNIT_TESTS} ${SOURCES})
target_link_libraries(unit_tests PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
