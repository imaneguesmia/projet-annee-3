#include "position.hpp"

#include <stdexcept>
#include <format>
#include <cstring>

/* -- DEFINE class Position -- */

void Position::setPosition(int row, int col) {
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        throw std::invalid_argument(std::format("Invalid position: ({}, {})", row, col));
    }

    position_index = Square((row * 8) + col);
}

void Position::setPositionIndex(int index) {
    if (index < Position::a8 || index > Position::Invalid) {
        throw std::invalid_argument(std::format("Invalid position index: {}", index));
    }

    position_index = index;
}

void Position::getPositionString(char out[3]) const {
    if (!isValid()) {
        strcpy(out, "-");
    } else {
        int row = position_index / 8;
        int col = position_index % 8;

        out[0] = 'a' + col;
        out[1] = '8' - row;
        out[2] = '\0';
    }
};

void Position::setPositionString(const char string[3]) {
    if (
        ('a' > string[0] || 'h' < string[0]) && string[0] != '-' ||
        ('1' > string[1] || '8' < string[1]) && string[1] != '\0' ||
        string[2] != '\0'
    ) {
        throw std::invalid_argument(std::format("Invalid position string: {}", string));
    }

    if (strcmp(string, "-") == 0) {
        setPositionInvalid();
    } else {
        int row = '8' - string[1];
        int col = string[0] - 'a';

        position_index = Square(row*8 + col);
    }
}

std::ostream& operator<<(std::ostream& out, const Position& position) {
    char string[3];
    position.getPositionString(string);

    out << string;
    return out;
}

/* -- END DEFINE -- */