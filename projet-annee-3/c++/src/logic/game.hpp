#pragma once

#include "game_data.hpp"

#include "game_state.hpp"

#include "board.hpp"
#include "board_analysis.hpp"
#include "move_generator.hpp"
#include "player.hpp"
#include "position.hpp"
#include "piece.hpp"
#include "attack_tables.hpp"

#include "../view/board_view.hpp"

#include "../minimax/ZobristHash.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <stdexcept>
#include <stack>


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

    uint64_t board_hash;            // Zobrist hash of the previous position.
};

std::ostream& operator<<(std::ostream& out, const UnmakeMove& unmake_move);

/* ---- END DECLARE ---- */



/* ---- DECLARE class Game ---- */

class Game : public ExtendedGameData {
    const BoardAnalysis board_analysis;
    const MoveGenerator move_generator;

    const ZobristHash zobrist;

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

    /* -- Lazy board data -- */

    // Vector of all possible pseudo-legal moves from the current board position.
    std::vector<Move> current_pseudo_legals;
    // Vector of all possible legal moves from the current board position.
    std::vector<Move> current_legals;
    // `true` if the current player is in check.
    bool is_current_player_in_check;
    // Enum representing the win state of the game.
    GameState game_state {GameState::INGAME};
    // Zobrist hash of the current board position.
    uint64_t hash;


    // The above information will be updated if the relevant flag below is not set.
    enum LazyDataFlags {
        PSEUDO_LEGALS   = 0b00001,
        LEGALS          = 0b00010,
        IS_IN_CHECK     = 0b00100,
        GAME_STATE      = 0b01000,
        HASH            = 0b10000,
        ALL             = 0b11111
    };
    uint8_t update_flags {ALL};

    std::vector<UnmakeMove> unmake_move_list;


private:
    struct GameData {
        std::string board_string;
        Player current_player;
        uint8_t castling_rights;
        Position en_passant;
        size_t halfmoves, fullmoves;
    };
    const GameData gameStateFromFEN(const std::string& fen) const;

    Game(const std::shared_ptr<const AttackTables> at, const GameData& initial_state)
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

    Player getCurrentPlayer() const override { return current_player; };
    void setCurrentPlayer(const Player player) { current_player = player; };

    /* -- Getting board data -- */

    /**
     * @brief Gets the castling rights bits.
     * 
     * @return Bitflags indicating castling rights: `0b[qkQK]`.
     */
    uint8_t getCastlingRights() const override { return castling_rights; };

    // Gets the current valid en passant target.
    const Position& getEnPassantPosition() const override { return en_passant; };

    // Gets the list of pseudo-legal moves from the current board position.
    const std::vector<Move>& getCurrentPseudoLegals(bool only_captures = false) override;
    // Gets the list of legal moves from the current board position.
    const std::vector<Move>& getCurrentLegals(bool only_captures = false) override;

    // Returns `true` the current player's king is in check at the current board position.
    bool isCurrentlyInCheck() override;
    // Gets the current game state.
    GameState getGameState() override;

    Piece getPieceAt(const Position& position) const override {
        return board.pieceAt(position);
    }

    /* --  Data required by the AI -- */
    
    const Board& getBoard() const { return board; };

    // Gets the zobrist hash of the current board position.
    uint64_t getHash() override;

    bool isRepetition(int count = 2) override;
    bool isHalfMoveDraw() const override { return halfmoves >= 100; };

    /* -- (Un)doing moves -- */

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
    bool move(const Move move) override;

    /**
     * @brief Undoes the last made move.
     * 
     * @return The last made move, or `std::nullopt` if there are no moves to undo.
     */
    std::optional<Move> undoLastMove() override;

    /* -- String representation -- */

    // Translates the current board state into FEN notation.
    const std::string fen() const;

    void printBoard() const { std::cout << board << '\n'; };
};

/* ---- END DECLARE ---- */