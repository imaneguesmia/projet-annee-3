#include "board.hpp"

#include <sstream>
#include <format>
#include <cstring>

/* ---- DEFINE class Board ---- */

void Board::setPieceAt(const Position& position, const Piece& piece) {
    BB::set_bit(piece_bb[piece.getId()], position);

    BB::set_bit(occupancy_bb[int(piece.getPlayer())], position);
    BB::set_bit(occupancy_bb[2], position);
}

void Board::setPiecePositions(const std::string& piece_positions) {
    int position_index = 0;
    int row = 0;  // Mostly just to check that the given FEN is valid.

    for (const auto& c : piece_positions) {
        Piece piece = Piece(c);

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

// The difference between lower and upper case ASCII characters
#define TO_LOWER_CASE 32

Piece Board::pieceAt(const Position& position) const {
    for (int piece = 0; piece < 12; piece++) {
        BB::BitBoard bb = piece_bb[piece];

        if (BB::get_bit(bb, position)) return Piece::Id(piece);
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

    for (int i = 0; i < 4; i++) {
        const char castling_indicator_chars[4] {'K', 'Q', 'k', 'q'};

        if (castling_rights & (1 << i)) {
            out << castling_indicator_chars[i];
        }
    }

    /* En passant */

    out << ' ' << en_passant;

    /* Halfmoves and fullmoves */

    out << ' ' << halfmoves << ' ' << fullmoves;

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

    out << '\n' << board.fen(); 

    return out;
}

/* ---- END DEFINE ---- */