#include "piece.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <format>

#define TO_LOWER_CASE 32

Piece Piece::fromFen(char fen) {
    PType out;

    bool is_black = islower(fen);
    int type = fen - (TO_LOWER_CASE * is_black);

    switch (type) {
        case 'P': out = PType::Pawn; break;
        case 'N': out = PType::Knight; break;
        case 'B': out = PType::Bishop; break;
        case 'R': out = PType::Rook; break;
        case 'Q': out = PType::Queen; break;
        case 'K': out = PType::King; break;

        default: return Piece();
    }

    return Piece(out, is_black ? Player::Black : Player::White);
}

char Piece::fen() const {
    char out;

    switch (type) {
        case PType::Pawn: out = 'P'; break;
        case PType::Knight: out = 'N'; break;
        case PType::Bishop: out = 'B'; break;
        case PType::Rook: out = 'R'; break;
        case PType::Queen: out = 'Q'; break;
        case PType::King: out = 'K'; break;

        case PType::NoneType: return '.';
    }

    out += TO_LOWER_CASE * (player == Player::Black);
    return out;
}