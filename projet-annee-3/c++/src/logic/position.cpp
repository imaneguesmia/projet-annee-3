#include "position.hpp"

#include <stdexcept>
#include <format>
#include <cstring>

/* -- DEFINE class Position -- */

void Position::setPositionInvalid() {
    position_index = -1;

    position_string[0] = '-';
    position_string[1] = '-';
}

void Position::setPosition(int row, int col) {
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        throw std::invalid_argument(std::format("Invalid position: ({}, {})", row, col));
    }

    position_index = (row * 8) + col;

    position_string[0] = 'a' + col;
    position_string[1] = '8' - row;
}

void Position::setPositionIndex(int index) {
    if (index < -1 || index > 63) {
        throw std::invalid_argument(std::format("Invalid position index: {}", index));
    }

    if (index == -1) {
        setPositionInvalid();
    } else {
        position_index = index;

        int row = index / 8;
        int col = index % 8;

        position_string[0] = 'a' + col;
        position_string[1] = '8' - row;
    }
}

void Position::setPositionString(const char string[3]) {
    if (
        ('a' > string[0] || 'h' < string[0]) && string[0] != '-' ||
        ('1' > string[1] || '8' < string[1]) && string[1] != '-' ||
        string[2] != '\0'
    ) {
        throw std::invalid_argument(std::format("Invalid position string: {}", string));
    }

    if (strcmp(position_string, "--") == 0) {
        setPositionInvalid();
    } else {
        position_string[0] = string[0];
        position_string[1] = string[1];

        int row = '8' - string[1];
        int col = string[0] - 'a';

        position_index = row*8 + col;
    }
}

std::ostream& operator<<(std::ostream& out, const Position& position) {
    char string[3];
    position.getPositionString(string);

    out << string;
    return out;
}

/* -- END DEFINE -- */