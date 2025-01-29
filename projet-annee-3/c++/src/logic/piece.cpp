#include "piece.hpp"

#include <cctype>
#include <iostream>

#define TO_LOWER_CASE 32

Piece::P Piece::toP(char fen) {
    int out;

    bool is_black = islower(fen);
    int type = fen - (TO_LOWER_CASE * is_black);

    switch (type) {
        case 'P': out = P::W_Pawn; break;
        case 'N': out = P::W_Knight; break;
        case 'B': out = P::W_Bishop; break;
        case 'R': out = P::W_Rook; break;
        case 'Q': out = P::W_Queen; break;
        case 'K': out = P::W_King; break;

        default: return P::None;
    }

    out += 6 * is_black;
    return P(out);
}

char Piece::toChar(P piece) {
    char out;

    bool is_black = piece >= 6;
    int type = piece - (6 * is_black);

    switch (type) {
        case P::W_Pawn: out = 'P'; break;
        case P::W_Knight: out = 'N'; break;
        case P::W_Bishop: out = 'B'; break;
        case P::W_Rook: out = 'R'; break;
        case P::W_Queen: out = 'Q'; break;
        case P::W_King: out = 'K'; break;

        case P::None: return '.';
    }

    out += TO_LOWER_CASE * is_black;
    return out;
}