#include "board.hpp"

#include "../misc/safely_to_enum_class.hpp"
#include "../misc/increment_enum.hpp"

#include <sstream>
#include <format>
#include <cstring>

#include <fmt/core.h>

/* ---- DEFINE class Board ---- */

/* -- Helpers -- */

void Board::setPiecePositions(const std::string &piece_positions)
{
    int position_index = 0;
    int row = 0; // Mostly just to check that the given FEN is valid.

    for (const auto &c : piece_positions)
    {
        Piece piece = Piece::fromFen(c);

        if (piece.isNone())
        {
            // Not a piece
            switch (c)
            {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                position_index += c - '1'; // ASCII hack
                break;
            case '/':
                row++;
                continue;
            default:
                throw std::invalid_argument(fmt::v10::format(
                    "Invalid FEN sequence: unknown character '{}' in board definition", c));
                switch (c)
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                    position_index += c - '1'; // ASCII hack
                    break;
                case '/':
                    row++;
                    continue;
                default:
                    throw std::invalid_argument(fmt::format(
                        "Invalid FEN sequence: unknown character '{}' in board definition", c));
                }
            }
        }
        else
        {
            setPieceAt(safely_to_enum_class<Square>(position_index), piece);
        }

        if (position_index / 8 != row)
        {
            throw std::invalid_argument("Invalid FEN sequence : bad column count");
        }

        position_index++;
    }
}

BB::BitBoard Board::rookMovementWhenCastling(const Position &king_target) const
{
    BB::BitBoard rook_movement = 0ULL;

    switch (king_target.getPositionSquare())
    {
    case Square::g1: // White king's side
        BB::set_bit(rook_movement, static_cast<int>(Square::h1));
        BB::set_bit(rook_movement, static_cast<int>(Square::f1));
        break;
    case Square::c1: // White queen's side
        BB::set_bit(rook_movement, static_cast<int>(Square::a1));
        BB::set_bit(rook_movement, static_cast<int>(Square::d1));
        break;
    case Square::g8: // Black king's side
        BB::set_bit(rook_movement, static_cast<int>(Square::h8));
        BB::set_bit(rook_movement, static_cast<int>(Square::f8));
        break;
    case Square::c8: // Black queen's side
        BB::set_bit(rook_movement, static_cast<int>(Square::a8));
        BB::set_bit(rook_movement, static_cast<int>(Square::d8));
        break;
    default:
        break;
    }

    return rook_movement;
}

PType Board::_movePieceHelper(const Move move, std::optional<PType> captured_type)
{
    Player other_player = otherPlayer(move.player);

    BB::BitBoard from_bb = BB::new_at(move.source), to_bb = BB::new_at(move.target);
    BB::BitBoard fromTo_bb = from_bb | to_bb;

    PType final_captured_type = PType::NoneType;

    /* Move the moved piece */

    // Flip source and target bits in occupancy bitboards.
    piece_bb[move.player][move.p_type] ^= fromTo_bb;
    occupancy_bb[move.player] ^= fromTo_bb;

    // BB::out(std::cout, fromTo_bb);

    // If the destination has a piece (i.e. non-en passant capture), the target bit will be flipped to 0 here.
    // It will be flipped again back to 1 when treating captures.
    global_occupancy_bb ^= fromTo_bb;

    /* Handle castling */

    if (move.castle)
    {
        /* Move the rook after castling */

        BB::BitBoard rook_movement = rookMovementWhenCastling(move.target);

        // Flip source and target bits for rook movement.
        piece_bb[move.player][PType::Rook] ^= rook_movement;
        occupancy_bb[move.player] ^= rook_movement;
        global_occupancy_bb ^= rook_movement;
    }

    else
    {
        /* Handle the captured piece */

        if (move.capture)
        {
            /* Find final captured piece type */

            if (move.en_passant)
            {
                // Move the "captured piece" position up or down depending on the player color.
                to_bb = (to_bb << 8) >> (static_cast<int>(move.player) << 4);

                // Captured piece has to be a pawn.
                final_captured_type = PType::Pawn;
            }

            // If captured piece type is given, copy its value.
            else if (captured_type.has_value())
            {
                final_captured_type = captured_type.value();
            }
            // If captured piece type is not given, find it.
            else
            {
                for (PType type = PType::FIRST; type != PType::OOB; increment_enum(type))
                {
                    if (BB::get_bit(piece_bb[other_player][type], move.target))
                    {
                        final_captured_type = PType(type);

                        break;
                    }
                }
            }

            /* Remove captured piece */

            // Flip the captured piece bit in occupancy bitboards.
            piece_bb[other_player][final_captured_type] ^= to_bb;
            occupancy_bb[other_player] ^= to_bb;

            // If 0 at this bit previously (non-en passant capture, see above) flipped back to 1.
            global_occupancy_bb ^= to_bb;
        }

        /* Promotion */

        if (move.promotion != PType::NoneType)
        {
            // Replace pawn with the new piece type
            piece_bb[move.player][PType::Pawn] ^= to_bb;
            piece_bb[move.player][move.promotion] ^= to_bb;
        }
    }

    return final_captured_type;
}

/* -- Board operations -- */

Piece Board::pieceAt(const Position &position) const
{
    for (Player player = Player::FIRST; player != Player::OOB; increment_enum(player))
    {
        for (PType p_type = PType::FIRST; p_type != PType::OOB; increment_enum(p_type))
        {
            if (BB::get_bit(piece_bb[player][p_type], position))
                return Piece(p_type, Player(player));
        }
    }

    return Piece();
}

void Board::setPieceAt(const Position &position, const Piece &piece)
{
    BB::set_bit(piece_bb[piece.getPlayer()][piece.getType()], position);

    BB::set_bit(occupancy_bb[piece.getPlayer()], position);
    BB::set_bit(global_occupancy_bb, position);
}

std::string Board::getPositionString() const
{
    std::ostringstream out;

    Position position = 0;
    size_t blanks = 0;

    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            char piece{pieceAt(position++).fen()};

            if (piece != '.')
            {
                if (blanks)
                {
                    out << blanks;
                    blanks = 0;
                }

                out << piece;
            }
            else
            {
                blanks++;
            }
        }

        if (blanks)
        {
            out << blanks;
            blanks = 0;
        }

        if (row != 7)
            out << '/';
    }

    return out.str();
}

std::ostream &operator<<(std::ostream &out, const Board &board)
{
    Position position{0};

    out << "\n  a b c d e f g h\n";

    for (int row = 0; row < 8; row++)
    {
        out << (8 - row) << ' ';

        for (int column = 0; column < 8; column++)
        {
            out << board.pieceFenAt(position++) << ' ';
        }

        out << std::endl;
    }

    return out;
}

Position Board::kingSquare(Player player) const
{
    BB::BitBoard kingBB = bitboard(Piece(PType::King, player));
    if (kingBB == 0)
    {
        throw std::runtime_error("King not found on the board!");
    }
    return {safely_to_enum_class<Square>(BB::leastSignificantBitIndex(kingBB))};
}
/* ---- END DEFINE ---- */