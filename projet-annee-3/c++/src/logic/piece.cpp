#include "piece.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <format>

#define TO_LOWER_CASE 32

Piece Piece::fromFen(char fen) {
    Type out;

    bool is_black = islower(fen);
    int type = fen - (TO_LOWER_CASE * is_black);

    switch (type) {
        case 'P': out = Type::Pawn; break;
        case 'N': out = Type::Knight; break;
        case 'B': out = Type::Bishop; break;
        case 'R': out = Type::Rook; break;
        case 'Q': out = Type::Queen; break;
        case 'K': out = Type::King; break;

        default: return Piece();
    }

    return Piece(out, is_black ? Player::Black : Player::White);
}

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