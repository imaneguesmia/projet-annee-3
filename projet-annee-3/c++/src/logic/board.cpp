#include "board.hpp"

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
            setPieceAt(position_index, piece);
        }

        if (position_index / 8 != row) {
            throw std::invalid_argument("Invalid FEN sequence : bad column count");
        }

        position_index++;
    }
}

void Board::setCastlingRights(const std::string& castling_indicators) {
    if (castling_indicators[0] == '-') return;

    for (const auto& c : castling_indicators) {
        switch (c) {
            case 'K':
                castling_rights |= 0b0001;
            break;
            case 'Q':
                castling_rights |= 0b0010;
            break;
            case 'k':
                castling_rights |= 0b0100;
            break;
            case 'q':
                castling_rights |= 0b1000;
            break;
            default:
                throw std::invalid_argument(std::format(
                    "Invalid FEN sequence: unknown character '{}' in castling indicators"
                , c));
        }
    }
}

BB::BitBoard Board::rookMovementWhenCastling(const Position& king_target) const {
    BB::BitBoard rook_movement = 0ULL;

    switch (king_target) {
        case Position::g1:  // White king's side
            BB::set_bit(rook_movement, Position::h1);
            BB::set_bit(rook_movement, Position::f1);
        break;
        case Position::c1:  // White queen's side
            BB::set_bit(rook_movement, Position::a1);
            BB::set_bit(rook_movement, Position::d1);
        break;
        case Position::g8:  // Black king's side
            BB::set_bit(rook_movement, Position::h8);
            BB::set_bit(rook_movement, Position::f8);
        break;
        case Position::c8:  // Black queen's side
            BB::set_bit(rook_movement, Position::a8);
            BB::set_bit(rook_movement, Position::d8);
        break;
    }

    return rook_movement;
}

Board::Board(const std::string& fen) {
    std::istringstream ss(fen);

    /* Piece positions */

    std::string board;
    std::getline(ss, board, ' ');

    setPiecePositions(board);

    /* Current player */

    char player;
    ss >> player;

    setCurrentPlayer(player == 'w' ? Player::White : Player::Black);

    ss.ignore();  // Skip next whitespace

    /* Castling rights */

    std::string castling_indicators;
    std::getline(ss, castling_indicators, ' ');

    setCastlingRights(castling_indicators);

    /* En passant */

    if (ss.peek() != '-') {
        char position_string[3] {0};
        ss >> position_string[0] >> position_string[1];

        setEnPassantPosition(Position(position_string));
    } else {
        ss.ignore(2);
    }

    /* Halfmoves and fullmoves */

    ss >> halfmoves >> fullmoves;
}

void Board::setPieceAt(const Position& position, const Piece& piece) {
    BB::set_bit(piece_bb[int(piece.getPlayer())][piece.getType()], position);

    BB::set_bit(occupancy_bb[int(piece.getPlayer())], position);
    BB::set_bit(occupancy_bb[2], position);
}

void Board::makeMove(const Move move) {
    BB::BitBoard from_bb = BB::new_at(move.source), to_bb = BB::new_at(move.target);
    BB::BitBoard fromTo_bb = from_bb | to_bb;

    Piece piece {Piece::Id(move.piece)};
    Piece::Type p_type = piece.getType();

    Player player = piece.getPlayer();

    int player_index = int(player);
    int other_index = int(otherPlayer(player));

    piece_bb[player_index][p_type] ^= fromTo_bb;
    occupancy_bb[player_index] ^= fromTo_bb;

    if (move.capture) {
        // Determine the type of the captured piece.
        Piece::Type captured_type;

        if (move.en_passant) {
            // Move the "captured piece" position up or down depending on the player color
            to_bb = (to_bb >> 8) << (player_index << 4);
            
            // Remove the captured pawn from global occupancy bitboard.
            occupancy_bb[2] ^= to_bb;
            captured_type = Piece::Pawn;
        } else {
            // Find opposing piece that is captured
            for (int type = Piece::Pawn; type < Piece::NoneType; type++) {
                if (BB::get_bit(piece_bb[other_index][type], move.target)) {
                    captured_type = Piece::Type(type);

                    break;
                }
            }
        }

        // Remove the captured piece from the relevant bitboards.
        piece_bb[other_index][captured_type] ^= to_bb;
        occupancy_bb[other_index] ^= to_bb;

        // If capture, only remove the source from the global occupancy bitboard since the target
        // is occupied by the capturing piece. The only exception is en passant, which is taken
        // care of above.
        occupancy_bb[2] ^= from_bb;
        // Reset halfmoves on capture.
        halfmoves = 0;
    } else {
        // If no capture, move the piece on the global occupancy bitboard.
        occupancy_bb[2] ^= fromTo_bb;

        if (move.castle) {
            BB::BitBoard rook_movement = rookMovementWhenCastling(move.target);

            // Flip relevant occupancy bits for rook movement.
            piece_bb[player_index][Piece::Rook] ^= rook_movement;
            occupancy_bb[player_index] ^= rook_movement;
            occupancy_bb[2] ^= rook_movement;
        }

        if (move.double_push) {
            en_passant = move.target + (player == Player::White ? 8 : -8);
        } else {
            en_passant.setPositionInvalid();
        }

        // If pieces moved on the relevant squares, update castling rights accordingly.
        castling_rights &= castling_table[move.source] & castling_table[move.target];
        // Reset halfmoves on pawn move.
        halfmoves *= p_type != Piece::Pawn;
    }

    if (move.promotion != Piece::NoneId) {
        Piece promoted_to = Piece(Piece::Id(move.promotion));

        piece_bb[player_index][Piece::Pawn] ^= to_bb;
        piece_bb[player_index][promoted_to.getType()] ^= to_bb;
    }

    halfmoves++;
    fullmoves += player_index;
}

Piece Board::pieceAt(const Position& position) const {
    for (int i = 0; i < 2; i++) {
        for (int p_type = Piece::Pawn; p_type < Piece::NoneType; p_type++) {
            if (BB::get_bit(piece_bb[i][p_type], position)) 
                return Piece(Piece::Type(p_type), Player(i));
        }
    }

    return Piece::NoneId;
}

const std::string Board::fen() const {
    std::ostringstream out;
    size_t blanks {0};

    /* Piece positions */

    Position position {0};

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

    out << ' ' << (current_player == Player::White ? 'w' : 'b');

    /* Castling rights */

    out << ' ';

    if (castling_rights) {
        for (int i = 0; i < 4; i++) {
            const char castling_indicator_chars[4] {'K', 'Q', 'k', 'q'};

            if (castling_rights & (1 << i)) {
                out << castling_indicator_chars[i];
            }
        }
    } else {
        out << '-';
    }

    /* En passant */

    out << ' ' << en_passant;

    /* Halfmoves and fullmoves */

    out << ' ' << halfmoves << ' ' << fullmoves;

    return std::move(out.str());
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

    out << '\n' << board.fen(); 

    return out;
}

/* ---- END DEFINE ---- */