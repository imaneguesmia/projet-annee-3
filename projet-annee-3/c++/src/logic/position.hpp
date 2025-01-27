#include <iostream>

/* ---- DECLARE class Position ---- */

class Position {

public:
    // enum Index {
    //     a8, b8, c8, d8, e8, f8, g8, h8,
    //     a7, b7, c7, d7, e7, f7, g7, h7,
    //     a6, b6, c6, d6, e6, f6, g6, h6,
    //     a5, b5, c5, d5, e5, f5, g5, h5,
    //     a4, b4, c4, d4, e4, f4, g4, h4,
    //     a3, b3, c3, d3, e3, f3, g3, h3,
    //     a2, b2, c2, d2, e2, f2, g2, h2,
    //     a1, b1, c1, d1, e1, f1, g1, h1
    // };

private:
    int position_index;
    char position_string[2];

public:
    Position(int index) { setPositionIndex(index); };
    Position(const char string[3]) { setPositionString(string); };

    ~Position() {};

    /* -- Getters and setters -- */

    int getPositionIndex() const { return position_index; };
    void setPositionIndex(int index);

    void getPositionString(char out[3]) const {
        out[0] = position_string[0];
        out[1] = position_string[1];
        out[2] = '\0';
    };
    void setPositionString(const char string[3]);

    /* -- Increment/decrement operators -- */

    Position& operator++() {
        position_index++;
        return *this;
    };
    Position operator++(int) {
        Position previous = *this;
        operator++();
        return previous;
    };
    Position& operator--() {
        position_index--;
        return *this;
    };
    Position operator--(int) {
        Position previous = *this;
        operator--();
        return previous;
    };

    /* -- Typecasting -- */

    operator int() const { return position_index; };
};

std::ostream& operator<<(std::ostream& out, const Position& position);

/* ---- END DECLARE ---- */