#pragma once

#include "board.hpp"
#include "board_analysis.hpp"
#include "move_generator.hpp"
#include "player.hpp"
#include "position.hpp"
#include "piece.hpp"
#include "attack_tables.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <stdexcept>
#include <stack>

/* ---- DECLARE class InvalidMoveException ---- */

class InvalidMoveException : public std::invalid_argument {

public:
    InvalidMoveException(const std::string& what) : std::invalid_argument(what) {};
};

/* ---- END DECLARE ---- */



/* ---- DECLARE struct UnmakeMove ---- */

/**
 * @brief Struct containing undo data. Used for the make-unmake approach, and is more efficient
 * for undoing than saving a copy of the entire state.
 */
struct UnmakeMove {
    // The made move.
    Move move;

    // Irreversibles
    uint8_t castling_rights;
    Position en_passant;
    size_t halfmoves;
};

/* ---- END DECLARE ---- */



/* ---- DECLARE class Game ---- */

class Game {
    Board board;

    const BoardAnalysis board_analysis;
    const MoveGenerator move_generator;

    // Lookup table to handle castling rights changes.
    static constexpr uint8_t castling_table[64] = {
         7, 15, 15, 15,  3, 15, 15, 11,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        13, 15, 15, 15, 12, 15, 15, 14
    };

    Player current_player {Player::White};
    uint8_t castling_rights {0};            // Castling rights bits: `0b[qkQK]`
    Position en_passant;

    // Halfmoves are incremented at the start of each turn, and reset after each capture or pawn move.
    // Fullmoves are incremented after black's turn.
    size_t halfmoves, fullmoves;

    // Vector of all possible pseudo-legal moves from the current board position.
    // Used so that it doesn't have to be recalculated every time a human player makes
    // an invalid move.
    std::vector<Move> current_pseudo_legals;

    std::stack<UnmakeMove> unmake_move_list;

private:
    struct GameState {
        std::string board_string;
        Player current_player;
        uint8_t castling_rights;
        Position en_passant;
        size_t halfmoves, fullmoves;
    };
    const GameState gameStateFromFEN(const std::string& fen) const;

    Game(const std::shared_ptr<const AttackTables> at, const GameState& initial_state)
        : board_analysis(at)
        , move_generator(std::move(at))

        , board(initial_state.board_string)
        , current_player(initial_state.current_player)
        , castling_rights(initial_state.castling_rights)
        , en_passant(initial_state.en_passant)
        , halfmoves(initial_state.halfmoves)
        , fullmoves(initial_state.fullmoves)
    {};

    // Sets the current valid en passant target.
    void setEnPassantPosition(const Position& position) { en_passant = position; };
    // Switches the current player.
    void switchPlayer() { current_player = otherPlayer(current_player); };

    /**
     * @brief Gets the castling rights bits.
     * 
     * @return Bitflags indicating castling rights: `0b[qkQK]`.
     */
    uint8_t getCastlingRights() const { return castling_rights; };
    /**
     * @brief Calculates castling rights bits according to the given string.
     * 
     * @param castling_indicators   The string indicating castling rights, i.e. "KQq" => `0b1011`.
     * @returns Bitflags indicating castling rights: `0b[qkQK]`.
     */
    uint8_t castlingRightsFromString(const std::string& castling_indicators) const;

    /**
     * @brief Determines the source and target positions of the rook when castling.
     * 
     * @param king_target           The target position of the king when castling.
     * @return A bitboard indicating the source and target positions of the rook.
     */
    BB::BitBoard rookMovementWhenCastling(const Position& king_target) const;

    /**
     * @brief Makes the given move on the board, without testing legality.
     * 
     * @param move  The parameters of the move to make.
     */
    void makeMoveOnBoard(const Move move);

public:
    Game(const std::string& fen) : Game(
        std::make_shared<const AttackTables>(), 
        gameStateFromFEN(fen)
    ) {};

    /* -- Getters and setters -- */

    Player getCurrentPlayer() const { return current_player; };
    void setCurrentPlayer(const Player player) { current_player = player; };

    // Gets the current valid en passant target.
    const Position& getEnPassantPosition() const { return en_passant; };
    // Gets the list of pseudo-legal moves from the current board position.
    const std::vector<Move>& getCurrentPseudoLegals() const {
        return current_pseudo_legals;
    }

    /* -- -- */

    /**
     * @brief Makes a move on the board.
     * 
     * @param from          Source square of the move.
     * @param to            Target square of the move.
     * @param promoted_to   Piece to promote to, if applicable.
     * @returns `true` if the move is legal and was successfully made.
     */
    bool move(const Position& from, const Position& to, const Piece& promoted_to);
    /**
     * @brief Makes a move on the board.
     * 
     * @param move          The parameters of the move to make.
     * @returns `true` if the move is legal and was successfully made.
     */
    bool move(const Move move);

    /* -- -- */

    // Translates the current board state into FEN notation.
    const std::string fen() const;

    void printBoard() const { std::cout << board << '\n'; };
};

/* ---- END DECLARE ---- */