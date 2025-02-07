#include "game.hpp"

#include <sstream>

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

    return std::move(game_state);
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

void Game::makeMoveOnBoard(const Move move) {
    BB::BitBoard from_bb = BB::new_at(move.source), to_bb = BB::new_at(move.target);
    BB::BitBoard fromTo_bb = from_bb | to_bb;

    Piece piece {Piece::Id(move.piece)};
    Piece::Type p_type = piece.getType();

    Player player = piece.getPlayer();

    int player_index = int(player);
    int other_index = int(otherPlayer(player));

    board.piece_bb[player_index][p_type] ^= fromTo_bb;
    board.occupancy_bb[player_index] ^= fromTo_bb;

    if (move.capture) {
        // Determine the type of the captured piece.
        Piece::Type captured_type;

        if (move.en_passant) {
            // Move the "captured piece" position up or down depending on the player color
            to_bb = (to_bb >> 8) << (player_index << 4);
            
            // Remove the captured pawn from global occupancy bitboard.
            board.occupancy_bb[2] ^= to_bb;
            captured_type = Piece::Pawn;
        } else {
            // Find opposing piece that is captured
            for (int type = Piece::Pawn; type < Piece::NoneType; type++) {
                if (BB::get_bit(board.piece_bb[other_index][type], move.target)) {
                    captured_type = Piece::Type(type);

                    break;
                }
            }
        }

        // Remove the captured piece from the relevant bitboards.
        board.piece_bb[other_index][captured_type] ^= to_bb;
        board.occupancy_bb[other_index] ^= to_bb;

        // If capture, only remove the source from the global occupancy bitboard since the target
        // is occupied by the capturing piece. The only exception is en passant, which is taken
        // care of above.
        board.occupancy_bb[2] ^= from_bb;
        // Reset halfmoves on capture.
        halfmoves = 0;
    } else {
        // If no capture, move the piece on the global occupancy bitboard.
        board.occupancy_bb[2] ^= fromTo_bb;

        if (move.castle) {
            BB::BitBoard rook_movement = rookMovementWhenCastling(move.target);

            // Flip relevant occupancy bits for rook movement.
            board.piece_bb[player_index][Piece::Rook] ^= rook_movement;
            board.occupancy_bb[player_index] ^= rook_movement;
            board.occupancy_bb[2] ^= rook_movement;
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

        board.piece_bb[player_index][Piece::Pawn] ^= to_bb;
        board.piece_bb[player_index][promoted_to.getType()] ^= to_bb;
    }

    halfmoves++;
    fullmoves += player_index;
}

bool Game::move(const Position& from, const Position& to, const Piece& promoted_to) {
    
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

    return std::move(out.str());
}

/* ---- END DEFINE ---- */