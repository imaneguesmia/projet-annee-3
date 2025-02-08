#include "game.hpp"

#include "../misc/increment_enum.hpp"

#include <sstream>

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

/* ---- DEFINE class Game ---- */

const Game::GameState Game::gameStateFromFEN(const std::string& fen) const {
    std::istringstream ss(fen);

    GameState game_state;

    /* Piece positions */

    std::getline(ss, game_state.board_string, ' ');

    /* Current player */

    char player;
    ss >> player;

    game_state.current_player = (player == 'w' ? Player::White : Player::Black);

    ss.ignore();  // Skip next whitespace

    /* Castling rights */

    std::string castling_indicators;
    std::getline(ss, castling_indicators, ' ');

    game_state.castling_rights = castlingRightsFromString(castling_indicators);

    /* En passant */

    if (ss.peek() != '-') {
        char position_string[3] {0};
        ss >> position_string[0] >> position_string[1];

        game_state.en_passant = Position(position_string);
    } else {
        ss.ignore(2);
    }

    /* Halfmoves and fullmoves */

    ss >> game_state.halfmoves >> game_state.fullmoves;

    return game_state;
}

// Mem O'Sprite

uint8_t Game::castlingRightsFromString(const std::string& castling_indicators) const {
    uint8_t flags = 0b0000;

    if (castling_indicators[0] != '-') {
        for (const auto& c : castling_indicators) {
            switch (c) {
                case 'K':
                    flags |= 0b0001;
                break;
                case 'Q':
                    flags |= 0b0010;
                break;
                case 'k':
                    flags |= 0b0100;
                break;
                case 'q':
                    flags |= 0b1000;
                break;
                default:
                    throw std::invalid_argument(std::format(
                        "Invalid FEN sequence: unknown character '{}' in castling indicators"
                    , c));
            }
        }
    }

    return flags;
}

UnmakeMove Game::makeMoveOnBoard(const Move move) {
    /* Initialize UnmakeMove */

    UnmakeMove unmake_move;

    unmake_move.move = move;
    unmake_move.castling_rights = castling_rights;
    unmake_move.en_passant = en_passant;
    unmake_move.halfmoves = halfmoves;

    /* Move the piece(s) on the board */
    
    PType captured_type = board.movePiece(move);
    unmake_move.captured = captured_type;

    /* Double push en passant target */

    if (move.double_push) {
        en_passant = move.target + (move.player == Player::White ? 8 : -8);
    } else {
        en_passant.setPositionInvalid();
    }

    /* Update castling rights */

    castling_rights &= castling_table[move.source] & castling_table[move.target];

    /* Update halfmoves and fullmoves */

    if (move.capture || move.p_type == PType::Pawn) {
        halfmoves = 0;
    }

    halfmoves++;
    fullmoves += static_cast<int>(move.player);

    /* Update current player */

    current_player = otherPlayer(move.player);

    return unmake_move;
}

void Game::unmakeMoveOnBoard(const UnmakeMove unmake_move) {
    board.unmovePiece(unmake_move.move, unmake_move.captured);

    castling_rights = unmake_move.castling_rights;
    en_passant = unmake_move.en_passant;

    halfmoves = unmake_move.halfmoves;
    fullmoves -= static_cast<int>(unmake_move.move.player);
    
    current_player = unmake_move.move.player;
}

bool Game::move(const Position& from, const Position& to, const Piece& promoted_to) {
    
}

bool Game::move(const Move move) {
    UnmakeMove unmake_move = makeMoveOnBoard(move);

    // If the player's king is left in check after the move, it is invalid.
    bool is_invalid_move = board_analysis.isInCheck(move.player, board);

    if (is_invalid_move) {
        // If not valid, immediately undo the move.
        unmakeMoveOnBoard(unmake_move);
    } else {
        // If valid, add the UnmakeMove to the history stack.
        unmake_move_list.push(unmake_move);
    }

    return !is_invalid_move;
}

std::optional<Move> Game::undoLastMove() {
    if (unmake_move_list.empty()) {
        return std::nullopt;
    } else {
        const UnmakeMove& unmake_move = unmake_move_list.top();
        unmake_move_list.pop();

        unmakeMoveOnBoard(unmake_move);

        return unmake_move.move;
    }
}

const std::string Game::fen() const {
    std::ostringstream out;

    /* Piece positions */

    out << board.getPositionString();

    /* Player */

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

    return out.str();
}

/* ---- END DEFINE ---- */