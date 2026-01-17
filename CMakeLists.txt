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
# Sources
# -----------------------------
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/engine/*.cpp)
file(GLOB_RECURSE GAME_SOURCES   CONFIGURE_DEPENDS src/game/*.cpp)

# web 只收「API 的 cpp」，不要把 server.cpp 也丟進去
file(GLOB_RECURSE WEB_SOURCES    CONFIGURE_DEPENDS src/web/*.cpp)
list(FILTER WEB_SOURCES EXCLUDE REGEX ".*/server\\.cpp$")

set(COMMON_SOURCES ${ENGINE_SOURCES} ${GAME_SOURCES})
set(ALL_SOURCES ${COMMON_SOURCES} ${WEB_SOURCES})

# -----------------------------
# Game executable (main program)
# -----------------------------
add_executable(game src/main.cpp ${ALL_SOURCES})

# -----------------------------
# Server executable
# -----------------------------
add_executable(server src/web/server.cpp ${ALL_SOURCES})

target_include_directories(server PRIVATE
    src
    src/engine
    src/game
    src/web
    external
)

# （可選）如果 game 也需要 include web headers，可加，但通常不需要 external
target_include_directories(game PRIVATE
    src
    src/engine
    src/game
    src/web
)

# -----------------------------
# Unit tests
# -----------------------------
file(GLOB_RECURSE UNIT_TESTS CONFIGURE_DEPENDS tests/*.cpp)

add_executable(unit_tests ${UNIT_TESTS} ${ALL_SOURCES})
target_link_libraries(unit_tests PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
