#include "board.hpp"

#include "../misc/safely_to_enum_class.hpp"
#include "../misc/increment_enum.hpp"

#include <sstream>
#include <format>
#include <cstring>

/* ---- DEFINE class Board ---- */

void Board::setPiecePositions(const std::string& piece_positions) {
    int position_index = 0;
    int row = 0;  // Mostly just to check that the given FEN is valid.

    for (const auto& c : piece_positions) {
        Piece piece = Piece::fromFen(c);

        if (piece.isNone()) {
            // Not a piece
            switch (c) {
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
                    position_index += c - '1';  // ASCII hack
                    break;
                case '/':
                    row++;
                    continue;
                default:
                    throw std::invalid_argument(std::format(
                        "Invalid FEN sequence: unknown character '{}' in board definition"
                    , c));
            }
        } else {
            setPieceAt(safely_to_enum_class<Square>(position_index), piece);
        }

        if (position_index / 8 != row) {
            throw std::invalid_argument("Invalid FEN sequence : bad column count");
        }

        position_index++;
    }
}

Piece Board::pieceAt(const Position& position) const {
    for (Player player = Player::FIRST; player != Player::OOB; increment_enum(player)) {
        for (PType p_type = PType::FIRST; p_type != PType::OOB; increment_enum(p_type)) {
            if (BB::get_bit(piece_bb[player][p_type], position)) 
                return Piece(p_type, Player(player));
        }
    }

    return Piece();
}

void Board::setPieceAt(const Position& position, const Piece& piece) {
    BB::set_bit(piece_bb[piece.getPlayer()][piece.getType()], position);

    BB::set_bit(occupancy_bb[piece.getPlayer()], position);
    BB::set_bit(global_occupancy_bb, position);
}

void Board::movePieceByBitboard(const Piece& piece, const BB::BitBoard fromTo_bb) {
    // piece_bb[int(piece.getPlayer())][piece.getType()]
}

std::string Board::getPositionString() const {
    std::ostringstream out;

    Position position = 0;
    size_t blanks = 0;

    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            char piece {pieceAt(position++).fen()};

            if (piece != '.') {
                if (blanks) {
                    out << blanks;
                    blanks = 0;
                }

                out << piece;
            }
            else {
                blanks++;
            }
        }

        if (blanks) {
            out << blanks;
            blanks = 0;
        }
        
        if (row != 7) out << '/';
    }

    return out.str();
}

std::ostream& operator<<(std::ostream& out, const Board& board) {
    Position position {0};

    out << "\n  a b c d e f g h\n";

    for (int row = 0; row < 8; row++) {
        out << (8 - row) << ' ';

        for (int column = 0; column < 8; column++) {
            out << board.pieceFenAt(position++) << ' ';
        }

        out << std::endl;
    }

    out << '\n' << board.getPositionString(); 

    return out;
}

/* ---- END DEFINE ---- */