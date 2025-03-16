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

bool operator==(const Move& lhs, const Move& rhs) {
    return (
        lhs.source == rhs.source &&
        lhs.target == rhs.target &&
        lhs.player == rhs.player &&
        lhs.p_type == rhs.p_type &&
        lhs.promotion == rhs.promotion &&
        lhs.capture == rhs.capture &&
        lhs.double_push == rhs.double_push &&
        lhs.en_passant == rhs.en_passant &&
        lhs.castle == rhs.castle
    );
}

bool operator!=(const Move& lhs, const Move& rhs) {
    return !(lhs == rhs);
}