#include "board.hpp"

#include <sstream>
#include <format>
#include <cstring>

/* ---- DEFINE class Board ---- */

void Board::setPiecePositions(const std::string& piece_positions) {
    int position_index = 0;
    int row = 0;  // Mostly just to check that the given FEN is valid.

    for (const auto& c : piece_positions) {
        switch (c) {
            case W_Pawn:
                BB::set_bit(w_pawn, position_index);
                break;
            case W_Knight:
                BB::set_bit(w_knight, position_index);
                break;
            case W_Bishop:
                BB::set_bit(w_bishop, position_index);
                break;
            case W_Rook:
                BB::set_bit(w_rook, position_index);
                break;
            case W_Queen:
                BB::set_bit(w_queen, position_index);
                break;
            case W_King:
                BB::set_bit(w_king, position_index);
                break;
            case B_Pawn:
                BB::set_bit(b_pawn, position_index);
                break;
            case B_Knight:
                BB::set_bit(b_knight, position_index);
                break;
            case B_Bishop:
                BB::set_bit(b_bishop, position_index);
                break;
            case B_Rook:
                BB::set_bit(b_rook, position_index);
                break;
            case B_Queen:
                BB::set_bit(b_queen, position_index);
                break;
            case B_King:
                BB::set_bit(b_king, position_index);
                break;
            
            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8':
                // ASCII hack; we subtract 1 extra to compensate for the increment
                position_index += c - '1';
                break;
            
            case '/':
                row++;
                continue;
            
            default:
                throw std::invalid_argument(std::format("Invalid FEN sequence: unknown character '{}'", c));
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

char Board::pieceAt(const Position& position) const {
    char out;

    if (isOccupied(position)) {
        if (isBlack(position)) out += TO_LOWER_CASE;  // More ASCII hack

        if (isPawn(position)) out += W_Pawn;
        else if (isKnight(position)) out += W_Knight;
        else if (isBishop(position)) out += W_Bishop;
        else if (isRook(position)) out += W_Rook;
        else if (isQueen(position)) out += W_Queen;
        else if (isKing(position)) out += W_King;
    }
    else out = None;

    return out;
}

const std::string Board::fen() const {
    std::ostringstream out;
    size_t blanks {0};

    /* Piece positions */

    Position position {0};

    for (int row = 0; row < 8; row++) {
        for (int column = 0; column < 8; column++) {
            char piece {pieceAt(position++)};

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
            out << board.pieceAt(position++) << ' ';
        }

        out << std::endl;
    }

    out << '\n' << board.fen(); 

    return out;
}

/* ---- END DEFINE ---- */