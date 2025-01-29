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

        if (piece.id() == Piece::None) {
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
                        "Invalid FEN sequence: unknown character '{}'"
                    , c));
            }
        } else {
            BB::set_bit(piece_bb[piece.id()], position_index);
        }

        if (position_index / 8 != row) {
            throw std::invalid_argument("Invalid FEN sequence : bad column count");
        }

        position_index++;
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

    setCurrentPlayer(Player(player));

    /* Castling rights */

    ss >> castling_rights;

    /* En passant */

    char position_string[3] {0};
    ss >> position_string[0] >> position_string[1];

    setEnPassantPosition(Position(position_string));

    /* Halfmoves and fullmoves */

    ss >> halfmoves >> fullmoves;
}

// The difference between lower and upper case ASCII characters
#define TO_LOWER_CASE 32

Piece Board::pieceAt(const Position& position) const {
    for (int piece = 0; piece < 12; piece++) {
        BB::BitBoard bb = piece_bb[piece];

        if (BB::get_bit(bb, position)) return Piece::fromId(piece);
    }

    return Piece::None;
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

    out << ' ' << char(current_player);

    /* Castling rights */

    out << ' ' << castling_rights;

    /* En passant */

    out << ' ' << en_passant;

    /* Halfmoves and fullmoves */

    out << ' ' << halfmoves << ' ' << fullmoves;

    return out.str();
}

std::ostream& operator<<(std::ostream& out, const Board& board) {
    Position position {0};

    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            out << board.pieceFenAt(position++) << ' ';
        }

        out << std::endl;
    }

    out << '\n' << board.fen(); 

    return out;
}

/* ---- END DEFINE ---- */