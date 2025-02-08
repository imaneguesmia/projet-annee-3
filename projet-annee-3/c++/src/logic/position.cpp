#include "position.hpp"

#include "../misc/safely_to_enum_class.hpp"
#include "../misc/increment_enum.hpp"

#include <stdexcept>
#include <format>
#include <cstring>

/* -- DEFINE class Position -- */

Position::Position(int index) {
    if (index == static_cast<int>(Square::Invalid)) setPositionInvalid();
    else setPositionIndex(safely_to_enum_class<Square>(index));
}

void Position::setPosition(int row, int col) {
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        throw std::invalid_argument(std::format("Invalid position: ({}, {})", row, col));
    }

    position_index = Square((row * 8) + col);
}

void Position::getPositionString(char out[3]) const {
    if (!isValid()) {
        strcpy(out, "-");
    } else {
        out[0] = 'a' + getColumn();
        out[1] = '8' - getRow();
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

Position& Position::operator++() {
    increment_enum(position_index);
    return *this;
};
Position Position::operator++(int) {
    Position previous = *this;
    operator++();
    return previous;
};

// Decrements the position to the previous index, i.e. e7->d7, a5->h6.

Position& Position::operator--() {
    decrement_enum(position_index);
    return *this;
};
Position Position::operator--(int) {
    Position previous = *this;
    operator--();
    return previous;
};

std::ostream& operator<<(std::ostream& out, const Position& position) {
    char string[3];
    position.getPositionString(string);

    out << string;
    return out;
}

/* -- END DEFINE -- */