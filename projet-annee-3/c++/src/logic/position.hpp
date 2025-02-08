#pragma once

#include <utility>
#include <iostream>

// Enum over board positions.
enum class Square : uint8_t {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,

    Invalid,  // Special value indicating an invalid position.

    FIRST = a8, LAST = h1
};

/* ---- DECLARE class Position ---- */

/**
 * @brief Class representing a position (square) on the board.
 */
class Position {

private:
    Square position_index;  // The index of the position, from 0 to 63. 64 if invalid.

public:
    // Constructs a Position from the given row and column.
    Position(int row, int col) { setPosition(row, col); };
    // Constructs a Position with the given index.
    Position(Square index) { setPositionIndex(index); };
    // Constructs a Position with the given index. Throws `std::invalid_argument` if the index is out of bounds.
    Position(int index);
    // Constructs a Position from the given position string, i.e. "e4". "-" if invalid.
    Position(const char string[3]) { setPositionString(string); };

    // Constructs an invalid Position.
    Position() : Position(Square::Invalid) {};

    ~Position() {};

    // Sets this position as an "invalid" (out-of-bounds or non-existent) position.
    void setPositionInvalid() { position_index = Square::Invalid; };
    // Returns `true` if the position is valid.
    bool isValid() const { return position_index != Square::Invalid; };

    /* -- Getters and setters -- */

    // Gets the row of the position. Returns meaningless value if position is invalid.
    int getRow() const { return std::to_underlying(position_index) / 8; };
    // Gets the column of the position. Returns meaningless value if position is invalid.
    int getColumn() const { return std::to_underlying(position_index) % 8; };

    // Sets the position to the given row and column.
    void setPosition(int row, int col);

    Square getPositionIndex() const { return position_index; };
    void setPositionIndex(Square index) { position_index = index; };

    Square getPositionSquare() const { return Square(position_index); };

    // Gets the string representation of the position, i.e. "e4". "-" if invalid.
    void getPositionString(char out[3]) const;
    // Sets the position from the given position string.
    void setPositionString(const char string[3]);

    /* -- Increment/decrement operators -- */

    // Increments the position to the next index, i.e. a7->b7, h3->a2.

    Position& operator++();
    Position operator++(int);

    Position& operator--();
    Position operator--(int);

    /* -- Typecasting -- */

    operator uint8_t() const { return std::to_underlying(position_index); };
};

// Prints the string representation of the position to the output stream.
std::ostream& operator<<(std::ostream& out, const Position& position);

/* ---- END DECLARE ---- */