cmake_minimum_required(VERSION 3.15...3.31)
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")

project(
    ProjetAnnee3
    VERSION 1.0
    DESCRIPTION "chess go brr"
    LANGUAGES CXX
)


# Forcer C++23 partout
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Directories
set(LOGIC "logic")
set(MISC "misc")

# Compiler options (ajout de -stdlib=libc++ pour macOS)
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


# Libraries
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

# Find Python and nanobind
set(Python_FIND_VIRTUALENV FIRST)
find_package(Python 3.13 REQUIRED
    COMPONENTS          Interpreter Development.Module
    OPTIONAL_COMPONENTS Development.SABIModule
)
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR)
list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")
find_package(nanobind REQUIRED)

# Create the nanobind module
nanobind_add_module(chess_module "src/export.cpp")
target_link_libraries(chess_module PRIVATE logic misc)

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

# Comment out the next lines if you don't want to build the testing executable
add_executable(testing "src/testing.cpp")
target_link_libraries(testing PRIVATE logic)
target_link_libraries(testing PRIVATE misc)


