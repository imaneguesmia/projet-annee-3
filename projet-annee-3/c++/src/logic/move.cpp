#include "move.hpp"

#include "position.hpp"
#include "piece.hpp"

std::ostream& operator<<(std::ostream& out, const Move& move) {
    out << Position(move.source) << "->" << Position(move.target) << '\n';

    Piece piece_obj {Piece::Id(move.piece)};

    out << "Piece :       " << Piece(Piece::Id(move.piece)).fen() << '\n';
    out << "Promoted ?    " << Piece(Piece::Id(move.promotion)).fen() << '\n';
    out << "Capture ?     " << move.capture << '\n';
    out << "Double push ? " << move.double_push << '\n';
    out << "En passant ?  " << move.en_passant << '\n';
    out << "Castle ?      " << move.castle << '\n';

    return out;
}