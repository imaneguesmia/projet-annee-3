#pragma once

// enum class Piece {
//     W_Pawn = 0b0000, W_Knight, W_Bishop,
//     W_Rook, W_Queen, W_King,

//     B_Pawn = 0b1000, B_Knight, B_Bishop,
//     B_Rook, B_Queen, B_King,

//     None
// };

class Piece {

public:
    enum P {
        W_Pawn, W_Knight, W_Bishop,
        W_Rook, W_Queen, W_King,

        B_Pawn, B_Knight, B_Bishop,
        B_Rook, B_Queen, B_King,

        None = -1
    };

private:
    const P type;

    Piece(char fen) : type(toP(fen)) {};

    static P toP(char fen);
    static char toChar(P piece);

public:
    ~Piece() {};

    Piece(P type) : type(type) {};

    static Piece fromId(int id) { return Piece(P(id)); };
    static Piece fromFen(char fen) { return Piece(fen); };

    P id() const { return type; };
    char fen() const { return toChar(type); };

    bool isBlack() const { return type & 0b1000; };
};