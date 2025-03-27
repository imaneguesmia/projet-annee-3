cmake_minimum_required(VERSION 3.20...3.31)

project(
    ProjetAnnee3
    VERSION 1.0
    DESCRIPTION "chess go brr"
    LANGUAGES CXX
)

# Use c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O2")

# Directories
set(LOGIC "logic")
set(MISC "misc")
set(VIEW "view")
set(MINIMAX "minimax")
set(MANAGEMENT "game_management")

# Compiler options (add -stdlib=libc++ for macOS)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    MESSAGE("Compiling with Clang/AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# ---- fmt ---- #

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

link_libraries(fmt::fmt)

# ---- Libraries ---- #

# Library for all game logic functionality
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
    "src/${LOGIC}/unmake_move.cpp" "src/${LOGIC}/unmake_move.hpp"
)
target_compile_options(logic PRIVATE -Wall -Wextra -Wpedantic)

# Library for all "view" functionality, i.e. data access for display purposes.
add_library(
    view STATIC 
    "src/${VIEW}/board_view.cpp" "src/${VIEW}/board_view.hpp"
)
target_compile_options(view PRIVATE -Wall -Wextra -Wpedantic)

add_library(
    misc STATIC
    "src/${MISC}/enum_array.hpp"
    "src/${MISC}/increment_enum.hpp"
    "src/${MISC}/safely_to_enum_class.hpp"
    "src/${MISC}/random64.cpp" "src/${MISC}/random64.hpp"
)

# Comment out this line if you want to use the pregenerated magics
target_compile_definitions(logic PRIVATE FIND_NEW_MAGICS)

# Library for all AI functionality
add_library(
    minimax STATIC
    "src/${MINIMAX}/Search.cpp" "src/${MINIMAX}/Search.hpp"
    "src/${MINIMAX}/TranspositionTable.cpp" "src/${MINIMAX}/TranspositionTable.hpp"
    "src/${MINIMAX}/MoveOrdering.cpp" "src/${MINIMAX}/MoveOrdering.hpp"
    "src/${MINIMAX}/Evaluator.cpp" "src/${MINIMAX}/Evaluator.hpp"
    "src/${MINIMAX}/IEvaluator.hpp"
    "src/${MINIMAX}/NNUEEvaluator.cpp" "src/${MINIMAX}/NNUEEvaluator.hpp"
    "src/${MINIMAX}/evaluator_settings.hpp"
    "src/${MINIMAX}/QuiescenceSearch.cpp" "src/${MINIMAX}/QuiescenceSearch.hpp"
    "src/${MINIMAX}/ZobristHash.cpp" "src/${MINIMAX}/ZobristHash.hpp"
    "src/${MINIMAX}/KingSafety.cpp" "src/${MINIMAX}/KingSafety.hpp"

)

# Library for game management
add_library(
    game_management STATIC
    "src/${MANAGEMENT}/game_manager.cpp" "src/${MANAGEMENT}/game_manager.hpp"
    "src/${MANAGEMENT}/move_accepter.hpp"
    "src/${MANAGEMENT}/move_prompter.hpp"
)

set(LIBS "logic misc view minimax game_management")

# ---- Create Python bindings ---- #

# Find Python and nanobind
set(Python_FIND_STRATEGY LOCATION)
set(Python_FIND_VIRTUALENV FIRST)
find_package(Python 3.13 REQUIRED
    COMPONENTS          Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.SABIModule
)
message(STATUS ${Python_EXECUTABLE})
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR)
list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")
find_package(nanobind REQUIRED)

# Create the nanobind module
nanobind_add_module(chess_module "src/export.cpp")
target_link_libraries(chess_module PRIVATE logic misc view minimax game_management)

# Python typing
nanobind_add_stub(
    chess_module_stub
    MODULE chess_module
    OUTPUT chess_module.pyi
    PYTHON_PATH $<TARGET_FILE_DIR:chess_module>
    DEPENDS chess_module
)

# Install the Python module
set(CMAKE_INSTALL_PREFIX "../..")
install(TARGETS chess_module DESTINATION cpp_chess)
install(FILES "build/chess_module.pyi" DESTINATION cpp_chess)

# ---- Testing executable ---- #

# Comment out the next lines if you don't want to build the testing executable
add_executable(testing "src/testing.cpp")
target_link_libraries(testing PRIVATE logic misc view minimax game_management)