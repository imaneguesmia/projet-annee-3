#pragma once

#include "../game_management/move_provider.hpp"

#include "evaluator_settings.hpp"
#include "TranspositionTable.hpp"
#include "Evaluator.hpp"
#include "MoveOrdering.hpp"

#include "../logic/game_data.hpp"
#include "../logic/attack_tables.hpp"

#include <memory>

/**
 * @class Beluga
 * @brief Implements the NegaMax search with alpha-beta pruning and aspiration windows.
 */
class Beluga : public AIMoveProvider {
public:
    /**
     * @brief Constructor for the Beluga search engine.
     * @param depth Maximum search depth.
     */
    explicit Beluga(int depth, EvaluatorSettings eval);

    /**
     * @brief Determines the best move for the current board position.
     * @param board The current chess board state.
     * @return The best move found.
     */
    Move getMove(ExtendedGameData& board) override;

    /**
     * @brief Evaluates the current board position.
     * @param board The current chess board state.
     * @return The ratio between the value of this state for the given player and the total value
     * of this state for both players
     */
    int getPositionValue(ExtendedGameData& board, Player player) override;

private:
    /**
     * @brief Implements the NegaMax search algorithm with alpha-beta pruning.
     * @param board The current board state.
     * @param depth Remaining search depth.
     * @param alpha Alpha bound for pruning.
     * @param beta Beta bound for pruning.
     * @param ply Current search depth from the root.
     * @return Evaluation score for the position.
     */
    int negamax(ExtendedGameData& board, int depth, int alpha, int beta, int ply);

    /**
     * @brief Evaluates a terminal game state (checkmate, stalemate, etc.).
     * @param end_state The game state at the end of the game.
     * @param ply Current depth in the search.
     * @return Evaluation score based on the game result.
     */
    int evaluateTerminal(GameState end_state, int ply) const;

private:
    int searchDepth;                 ///< Maximum search depth.
    TranspositionTable transpositionTable; ///< Transposition table for caching results.
    MoveOrdering moveOrdering;
    Evaluator evaluator;             ///< Static board evaluator.

    static constexpr int INF = 100000000;   ///< Representation of infinity for search values.
    static constexpr int MATE_SCORE = 1000000; ///< Score for a checkmate position.
    static constexpr int ASP_WIN = 50;      ///< Aspiration window value.
};
