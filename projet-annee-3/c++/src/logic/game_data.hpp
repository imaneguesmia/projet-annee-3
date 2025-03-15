#pragma once

#include "player.hpp"
#include "position.hpp"
#include "move.hpp"
#include "game_state.hpp"

#include <vector>
#include <optional>

class UnmakeMove;

/**
 * @brief Interface for game data.
 */
class GameData {

public:
    virtual ~GameData() {};

    virtual Player getCurrentPlayer() const = 0;
    virtual uint8_t getCastlingRights() const = 0;
    virtual const Position& getEnPassantPosition() const = 0;

    virtual const std::vector<Move>& getCurrentPseudoLegals(bool only_captures = false) = 0;
    virtual const std::vector<Move>& getCurrentLegals(bool only_captures = false) = 0;

    virtual bool isCurrentlyInCheck() = 0;
    virtual GameState getGameState() = 0;

    virtual Piece getPieceAt(const Position& position) const = 0;
};

class Board;

/**
 * @brief Interface for game data, with additional methods for obtaining data and
 * performing operations required by the AI.
 */
class ExtendedGameData : public GameData {

public:
    virtual ~ExtendedGameData() {};

    virtual bool move(Move move) = 0;
    virtual const UnmakeMove& getMostRecentUnmakeMove() const = 0;
    virtual std::optional<Move> undoLastMove() = 0;
    virtual void unmakeMoveOnBoard(const UnmakeMove& unmake_move) = 0;

    virtual const Board& getBoard() const = 0;

    // Gets the zobrist hash of the current board position.
    virtual uint64_t getHash() = 0;

    virtual bool isRepetition(int count) = 0;
    virtual bool isHalfMoveDraw() const = 0;
};