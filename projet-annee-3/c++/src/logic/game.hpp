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
    Move move;                      // The made move.
    PType captured          : 4;    // `PType` of captured piece (`PType::NoneType` if no capture).

    uint8_t castling_rights : 4;    // Bitflags indicating previous castling rights: `0b[qkQK]`.
    uint8_t en_passant;             // Previous valid en passant target.

    size_t halfmoves;               // Halfmove clock before the move.
};

std::ostream& operator<<(std::ostream& out, const UnmakeMove& unmake_move);

/* ---- END DECLARE ---- */



/* ---- DECLARE class Game ---- */

class Game {
    const BoardAnalysis board_analysis;
    const MoveGenerator move_generator;

    Board board;

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
    // The list of pseudo-legal moves will be lazily updated when being fetched if this flag
    // is set.
    bool board_position_changed {true};

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
     * @return Bitflags indicating castling rights: `0b[qkQK]`.
     */
    uint8_t castlingRightsFromString(const std::string& castling_indicators) const;

    /**
     * @brief Makes the given move on the board, without testing legality.
     * 
     * @param move  The parameters of the move to make.
     * @return The `UnmakeMove` object associated with this action.
     */
    UnmakeMove makeMoveOnBoard(const Move move);
    /**
     * @brief Unmakes a move.
     * 
     * @param unmake_move   The associated `UnmakeMove` object.
     */
    void unmakeMoveOnBoard(const UnmakeMove unmake_move);

public:
    Game(const std::string& fen) : Game(
        std::make_shared<const AttackTables>(), 
        gameStateFromFEN(fen)
    ) {};
    Game() : Game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};

    /* -- Getters and setters -- */

    Player getCurrentPlayer() const { return current_player; };
    void setCurrentPlayer(const Player player) { current_player = player; };

    // Gets the current valid en passant target.
    const Position& getEnPassantPosition() const { return en_passant; };

    /* -- (Un)doing moves -- */

    // Gets the list of pseudo-legal moves from the current board position.
    const std::vector<Move>& getCurrentPseudoLegals();
    // Gets the list of legal moves from the current board position.
    const std::vector<Move> getCurrentLegals();

    /**
     * @brief Makes a move on the board.
     * 
     * @param from          Source square of the move.
     * @param to            Target square of the move.
     * @param promoted_to   Piece type to promote to, if applicable.
     * @return `true` if the move is legal and was successfully made.
     */
    bool move(const Position& from, const Position& to, const PType promoted_to);
    /**
     * @brief Makes a move on the board.
     * 
     * @param move          The parameters of the move to make.
     * @return `true` if the move is legal and was successfully made. 
     */
    bool move(const Move move);

    /**
     * @brief Undoes the last made move.
     * 
     * @return The last made move, or `std::nullopt` if there are no moves to undo.
     */
    std::optional<Move> undoLastMove();

    /* -- Board info -- */

    Piece getPieceAt(const Position& position) const {
        return board.pieceAt(position);
    }

    /* -- String representation -- */

    // Translates the current board state into FEN notation.
    const std::string fen() const;

    void printBoard() const { std::cout << board << '\n'; };
};

/* ---- END DECLARE ---- */