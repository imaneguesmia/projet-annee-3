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

BB::BitBoard Game::rookMovementWhenCastling(const Position& king_target) const {
    BB::BitBoard rook_movement = 0ULL;

    switch (king_target.getPositionSquare()) {
        case Square::g1:  // White king's side
            BB::set_bit(rook_movement, static_cast<int>(Square::h1));
            BB::set_bit(rook_movement, static_cast<int>(Square::f1));
        break;
        case Square::c1:  // White queen's side
            BB::set_bit(rook_movement, static_cast<int>(Square::a1));
            BB::set_bit(rook_movement, static_cast<int>(Square::d1));
        break;
        case Square::g8:  // Black king's side
            BB::set_bit(rook_movement, static_cast<int>(Square::h8));
            BB::set_bit(rook_movement, static_cast<int>(Square::f8));
        break;
        case Square::c8:  // Black queen's side
            BB::set_bit(rook_movement, static_cast<int>(Square::a8));
            BB::set_bit(rook_movement, static_cast<int>(Square::d8));
        break;
        default: break;
    }

    return rook_movement;
}

UnmakeMove Game::makeMoveOnBoard(const Move move) {
    UnmakeMove unmake_move;

    unmake_move.move = move;
    unmake_move.castling_rights = castling_rights;
    unmake_move.en_passant = en_passant;
    unmake_move.halfmoves = halfmoves;

    BB::BitBoard from_bb = BB::new_at(move.source), to_bb = BB::new_at(move.target);
    BB::BitBoard fromTo_bb = from_bb | to_bb;

    Player other_player = otherPlayer(move.player);

    board.piece_bb[move.player][move.p_type] ^= fromTo_bb;
    board.occupancy_bb[move.player] ^= fromTo_bb;

    /* Double push en passant target */

    if (move.double_push) {
        en_passant = move.target + (move.player == Player::White ? 8 : -8);
    } else {
        en_passant.setPositionInvalid();
    }

    /* Promotion */

    if (move.promotion != PType::NoneType) {
        board.piece_bb[move.player][PType::Pawn] ^= to_bb;
        board.piece_bb[move.player][move.p_type] ^= to_bb;
    }

    /* Capture */

    if (move.capture) {
        en_passant.setPositionInvalid();

        // Determine the type of the captured piece.
        PType captured_type;

        if (move.en_passant) {
            // Move the "captured piece" position up or down depending on the player color
            to_bb = (to_bb << 8) >> (static_cast<int>(move.player) << 4);
            
            // Remove the captured pawn from global occupancy bitboard.
            board.global_occupancy_bb ^= to_bb;
            captured_type = PType::Pawn;
        } else {
            // Find opposing piece that is captured
            for (PType type = PType::FIRST; type != PType::OOB; increment_enum(type)) {
                if (BB::get_bit(board.piece_bb[other_player][type], move.target)) {
                    captured_type = PType(type);

                    break;
                }
            }
        }

        // Save the captured piece type.
        unmake_move.captured = captured_type;

        // Remove the captured piece from the relevant bitboards.
        board.piece_bb[other_player][captured_type] ^= to_bb;
        board.occupancy_bb[other_player] ^= to_bb;

        // If capture, only remove the source from the global occupancy bitboard since the target
        // is occupied by the capturing piece. The only exception is en passant, which is taken
        // care of above.
        board.global_occupancy_bb ^= from_bb;
        // Reset halfmoves on capture.
        halfmoves = 0;
    } else {
        // No capture.
        unmake_move.captured = PType::NoneType;

        // If no capture, move the piece on the global occupancy bitboard.
        board.global_occupancy_bb ^= fromTo_bb;

        if (move.castle) {
            BB::BitBoard rook_movement = rookMovementWhenCastling(move.target);

            // Flip relevant occupancy bits for rook movement.
            board.piece_bb[move.player][PType::Rook] ^= rook_movement;
            board.occupancy_bb[move.player] ^= rook_movement;
            board.global_occupancy_bb ^= rook_movement;
        }

        if (move.double_push) {
            en_passant = move.target + (move.player == Player::White ? 8 : -8);
        } else {
            en_passant.setPositionInvalid();
        }

        // If pieces moved on the relevant squares, update castling rights accordingly.
        castling_rights &= castling_table[move.source] & castling_table[move.target];
        // Reset halfmoves on pawn move.
        halfmoves *= move.p_type != PType::Pawn;
    }

    halfmoves++;
    fullmoves += static_cast<int>(move.player);

    return unmake_move;
}

void Game::unmakeMoveOnBoard(const UnmakeMove unmake_move) {
    castling_rights = unmake_move.castling_rights;
    en_passant = unmake_move.en_passant;
    halfmoves = unmake_move.halfmoves;


}

bool Game::move(const Position& from, const Position& to, const Piece& promoted_to) {
    
}

bool Game::move(const Move move) {
    UnmakeMove unmake_move = makeMoveOnBoard(move);

    std::cout << unmake_move << '\n';



    return true;
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