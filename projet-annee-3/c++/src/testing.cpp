/*
This is an entry point to test things. The actual final product will be a
C++ library and will not have any entry point.
*/

#include "logic/board.hpp"

#include <iostream>

int main(int argc, char ** argv) {
    std::cout << "Hello world !" << std::endl;

    Board b = Board::initialState();
    // Board b {"w"};

    std::cout << b << std::endl;

    return 0;
}