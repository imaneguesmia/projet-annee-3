#include "game.hpp"

#include "../misc/increment_enum.hpp"

#include <sstream>

#include <fmt/core.h>

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

/* -- Construction -- */

const Game::GameData Game::gameStateFromFEN(const std::string& fen) const {
    std::istringstream ss(fen);

    GameData game_state;

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

    std::string position_string;
    std::getline(ss, position_string, ' ');

    game_state.en_passant.setPositionString(position_string);

    /* Halfmoves and fullmoves */

    ss >> game_state.halfmoves >> game_state.fullmoves;

    return game_state;
}

/* -- Helpers -- */

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
                    throw std::invalid_argument(fmt::format(
                        "Invalid FEN sequence: unknown character '{}' in castling indicators"
                    , c));
            }
        }
    }

    return flags;
}

/* -- Getting board data -- */

const std::vector<Move>& Game::getCurrentPseudoLegals(bool only_captures) {
    if (update_flags & PSEUDO_LEGALS) {
        current_pseudo_legals = move_generator.generatePseudoLegals(
            current_player, board,
            en_passant, castling_rights,
            only_captures
        );

        update_flags ^= PSEUDO_LEGALS;
    }

    return current_pseudo_legals;
}

const std::vector<Move>& Game::getCurrentLegals(bool only_captures) {
    if (update_flags & LEGALS) {
        current_legals = move_generator.filterPseudoLegals(
            current_player, board,
            getCurrentPseudoLegals(only_captures)
        );

        update_flags ^= LEGALS;
    }

    return current_legals;
}

bool Game::isCurrentlyInCheck() {
    if (update_flags & IS_IN_CHECK) {
        is_current_player_in_check = board_analysis.isInCheck(current_player, board);

        update_flags ^= IS_IN_CHECK;
    }

    return is_current_player_in_check;
}

GameState Game::getGameState() {
    if (update_flags & GAME_STATE) {
        if (isHalfMoveDraw()) {
            game_state = GameState::HALF_MOVE_DRAW;
        } else if (isRepetition()) {
            game_state = GameState::REPETITION;
        } else if (getCurrentLegals().size() == 0) {
            game_state = isCurrentlyInCheck() ? GameState::CHECKMATE : GameState::STALEMATE;
        } else {
            game_state = GameState::INGAME;
        }

        update_flags ^= GAME_STATE;
    }

    return game_state;
}

uint64_t Game::getHash() {
    if (update_flags & HASH) {
        hash = zobrist.hash(*this);

        update_flags ^= HASH;
    }

    return hash;
}

bool Game::isRepetition(int count) {
    uint8_t c = 0;

    const int size = unmake_move_list.size();

    for (int i = size - 2; i >= 0 && i >= size - halfmoves; i -= 2) {
        if (unmake_move_list[i].board_hash == getHash()) c++;
        if (c == count) return true;
    }

    return false;
}


/* -- (Un)doing moves -- */

UnmakeMove Game::makeMoveOnBoard(const Move move) {
    /* Initialize UnmakeMove */

    UnmakeMove unmake_move;

    unmake_move.move = move;
    unmake_move.castling_rights = castling_rights;
    unmake_move.en_passant = en_passant;
    unmake_move.halfmoves = halfmoves;
    unmake_move.board_hash = getHash();

    /* Move the piece(s) on the board */
    
    PType captured_type = board.movePiece(move);
    unmake_move.captured = captured_type;

    /* Double push en passant target */

    if (move.double_push) {
        en_passant.setPositionSquare(move.target + (move.player == Player::White ? 8 : -8));
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
    en_passant.setPositionSquare(unmake_move.en_passant);

    halfmoves = unmake_move.halfmoves;
    fullmoves -= static_cast<int>(unmake_move.move.player);
    
    current_player = unmake_move.move.player;

    // NOTE: no need to update the zobrist hash here, it will be done lazily.
}

bool Game::move(const Position& from, const Position& to, const PType promoted_to) {
    for (const auto& possible_move : getCurrentPseudoLegals()) {
        if (
            possible_move.source == from && possible_move.target == to &&
            possible_move.promotion == promoted_to
        ) {
            return move(possible_move);
        }
    }

    return false;
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
        unmake_move_list.push_back(unmake_move);

        // Set update flags to update data.
        update_flags = ALL;
    }

    return !is_invalid_move;
}

std::optional<Move> Game::undoLastMove() {
    if (unmake_move_list.empty()) {
        return std::nullopt;
    } else {
        const UnmakeMove unmake_move = unmake_move_list.back();
        unmake_move_list.pop_back();

        undo(unmake_move);

        return unmake_move.move;
    }
}

void Game::undo(const UnmakeMove unmake_move) {
    unmakeMoveOnBoard(unmake_move);

    if (unmake_move_list.back().board_hash == unmake_move.board_hash) {
        unmake_move_list.pop_back();
    }

    // Set update flags to update data.
    update_flags = ALL;
}

/* -- String representation -- */

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