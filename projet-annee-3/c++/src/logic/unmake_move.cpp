#include "unmake_move.hpp"

/* ---- DEFINE struct UnmakeMove ---- */

std::ostream& operator<<(std::ostream& out, const UnmakeMove& unmake_move) {
    Piece captured {unmake_move.captured, otherPlayer(unmake_move.move.player)};

    out << ">>        Move        <<\n" << unmake_move.move << ">>  Unmake move data  <<\n";

    out << "Captured ?        " << captured.fen() << '\n';
    out << "Prev castle ?     " << int(unmake_move.castling_rights) << '\n';
    out << "Prev en passant ? " << Position(unmake_move.en_passant) << '\n';
    out << "Prev halfmoves ?  " << unmake_move.halfmoves << '\n';

    return out;
}

/* ---- END DEFINE ---- */