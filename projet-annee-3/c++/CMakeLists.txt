cmake_minimum_required(VERSION 3.15...3.31)

project(
    ProjetAnnee3
    VERSION 1.0
    DESCRIPTION "chess go brr"
    LANGUAGES CXX
)

# Directories

set(LOGIC "logic")
set(MISC "misc")

# Libraries
# NOTE : Even if it's annoying, please list EVERY source file required for each target!
#        It's much more reliable and less error-prone when working with bigger codebases
#        with multiple people.

add_library(
    logic STATIC 
    "src/${LOGIC}/bitboard.cpp" "src/${LOGIC}/bitboard.hpp"
    "src/${LOGIC}/position.cpp" "src/${LOGIC}/position.hpp"
    "src/${LOGIC}/player.hpp"
    "src/${LOGIC}/piece.cpp" "src/${LOGIC}/piece.hpp"
    "src/${LOGIC}/board.cpp" "src/${LOGIC}/board.hpp"
    "src/${LOGIC}/attack_tables.cpp" "src/${LOGIC}/attack_tables.hpp" "src/${LOGIC}/magic_numbers.hpp"
    "src/${LOGIC}/move.cpp" "src/${LOGIC}/move.hpp"
    "src/${LOGIC}/board_analysis.cpp" "src/${LOGIC}/board_analysis.hpp"
    "src/${LOGIC}/move_generator.cpp" "src/${LOGIC}/move_generator.hpp"
    "src/${LOGIC}/game.cpp" "src/${LOGIC}/game.hpp"
)
target_compile_features(logic PUBLIC cxx_std_23)
target_compile_options(logic PRIVATE -Wall -Wextra -Wpedantic)

add_library(
    misc INTERFACE
    "src/${MISC}/enum_array.hpp"
    "src/${MISC}/increment_enum.hpp"
    "src/${MISC}/safely_to_enum_class.hpp"
)

# Comment out this line if you want to use the pregenerated magics
# target_compile_definitions(logic PRIVATE FIND_NEW_MAGICS)

# Comment out the next lines if you don't want to build the testing executable
add_executable(testing "src/testing.cpp")
target_link_libraries(testing PRIVATE logic)
target_link_libraries(testing PRIVATE misc)