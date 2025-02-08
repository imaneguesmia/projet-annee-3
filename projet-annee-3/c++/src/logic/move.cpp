#include "move.hpp"

#include "player.hpp"

std::ostream& operator<<(std::ostream& out, const Move& move) {
    out << Position(move.source) << "->" << Position(move.target) << '\n';

    Piece piece_obj {move.p_type, move.player};

    out << "Piece :       " << piece_obj.fen() << '\n';
    out << "Promoted ?    " << Piece(move.promotion, otherPlayer(piece_obj.getPlayer())).fen() << '\n';
    out << "Capture ?     " << move.capture << '\n';
    out << "Double push ? " << move.double_push << '\n';
    out << "En passant ?  " << move.en_passant << '\n';
    out << "Castle ?      " << move.castle << '\n';

    return out;
}