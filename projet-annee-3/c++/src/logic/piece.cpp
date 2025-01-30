#include "piece.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <format>

#define TO_LOWER_CASE 32

Piece::Id Piece::fenToId(char fen) {
    int out;

    bool is_black = islower(fen);
    int type = fen - (TO_LOWER_CASE * is_black);

    switch (type) {
        case 'P': out = Type::Pawn; break;
        case 'N': out = Type::Knight; break;
        case 'B': out = Type::Bishop; break;
        case 'R': out = Type::Rook; break;
        case 'Q': out = Type::Queen; break;
        case 'K': out = Type::King; break;

        default: return Id::NoneId;
    }

    out += 6 * is_black;
    return Id(out);
}

// char Piece::idToFen(Id id) {
//     char out;

//     bool is_black = piece >= 6;
//     int type = piece - (6 * is_black);

//     switch (type) {
        // case P::W_Pawn: out = 'P'; break;
        // case P::W_Knight: out = 'N'; break;
        // case P::W_Bishop: out = 'B'; break;
        // case P::W_Rook: out = 'R'; break;
        // case P::W_Queen: out = 'Q'; break;
        // case P::W_King: out = 'K'; break;

        // case P::None: return '.';
//     }

//     out += TO_LOWER_CASE * is_black;
//     return out;
// }

char Piece::fen() const {
    char out;

    switch (type) {
        case Type::Pawn: out = 'P'; break;
        case Type::Knight: out = 'N'; break;
        case Type::Bishop: out = 'B'; break;
        case Type::Rook: out = 'R'; break;
        case Type::Queen: out = 'Q'; break;
        case Type::King: out = 'K'; break;

        case Type::NoneType: return '.';
    }

    out += TO_LOWER_CASE * (player == Player::Black);
    return out;
}