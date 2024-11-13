cmake_minimum_required(VERSION 3.15...3.31)

project(
    ProjetAnnee3
    VERSION 1.0
    DESCRIPTION "chess go brr"
    LANGUAGES CXX
)

# Directories

set(LOGIC "logic")

# Libraries
# NOTE : Even if it's annoying, please list EVERY source file required for each target!
#        It's much more reliable and less error-prone when working with bigger codebases
#        with multiple people.

add_library(logic STATIC "src/${LOGIC}/bitboard.cpp" "src/${LOGIC}/bitboard.hpp")

# Comment out the next lines if you don't want to build the testing executable

add_executable(testing "src/testing.cpp")
target_link_libraries(testing PRIVATE logic)