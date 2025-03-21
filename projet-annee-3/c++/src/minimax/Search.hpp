#pragma once

#include "../game_management/move_provider.hpp"

#include "evaluator_settings.hpp"
#include "TranspositionTable.hpp"
#include "IEvaluator.hpp"
#include "Evaluator.hpp"
#include "NNUEEvaluator.hpp"
#include "MoveOrdering.hpp"

#include "../logic/game_data.hpp"
#include "../logic/attack_tables.hpp"

#include <memory>
#include <string>

/**
 * @class Beluga
 * @brief Implements the NegaMax search with alpha-beta pruning and aspiration windows.
 */
class Beluga : public AIMoveProvider {
public:
    /**
     * @brief Constructor for the Beluga search engine.
     * @param depth Maximum search depth.
     * @param settings Settings for the evaluator used by the search.
     */
    explicit Beluga(int depth, EvaluatorSettings setting);

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
     * @param isPV Whether the current node is in the principal variation.
     * @param nullMoveAllowed Whether null move pruning is allowed in this position.
     * @return Evaluation score for the position.
     */
    int negamax(ExtendedGameData& board, int depth, int alpha, int beta, int ply, bool isPV = true, bool nullMoveAllowed = true);

    /**
     * @brief Evaluates a terminal game state (checkmate, stalemate, etc.).
     * @param end_state The game state at the end of the game.
     * @param ply Current depth in the search.
     * @return Evaluation score based on the game result.
     */
    int evaluateTerminal(GameState end_state, int ply) const;

    /**
     * @brief Checks if a move gives check to the opponent's king.
     * @param board The current board state.
     * @param move The move to check.
     * @return True if the move gives check, false otherwise.
     */
    bool givesCheck(ExtendedGameData& board, const Move& move);

private:
    int searchDepth;                 ///< Maximum search depth.
    TranspositionTable transpositionTable; ///< Transposition table for caching results.
    MoveOrdering moveOrdering;       ///< Move ordering utility.
    std::unique_ptr<IEvaluator> evaluator; ///< Static board evaluator (polymorphic).

    static constexpr int INF = 100000000;   ///< Representation of infinity for search values.
    static constexpr int MATE_SCORE = 1000000; ///< Score for a checkmate position.
    static constexpr int ASP_WIN = 50;      ///< Aspiration window value.
    
    // Late Move Reduction (LMR) constants
    static constexpr int LMR_DEPTH_THRESHOLD = 3;  ///< Minimum depth for LMR
    static constexpr int LMR_MOVES_THRESHOLD = 3;  ///< Number of moves to search before applying LMR
    static constexpr int LMR_BASE = 1;             ///< Base reduction amount
    static constexpr int LMR_MOVES_DIVISOR = 6;    ///< Divisor for calculating additional reduction
    static constexpr int LMR_MAX_REDUCTION = 3;    ///< Maximum additional reduction
    
    // Futility Pruning constants
    static constexpr int FUTILITY_DEPTH = 3;       ///< Maximum depth for futility pruning
    static constexpr int FUTILITY_MARGIN_BASE = 100; ///< Base margin for futility pruning
    static constexpr int FUTILITY_MARGIN_DEPTH = 150; ///< Additional margin per depth
    
    // Late Move Pruning (LMP) constants
    static constexpr int LMP_DEPTH = 3;            ///< Maximum depth for LMP
    static constexpr int LMP_BASE = 3;             ///< Base number of moves to search before LMP
    
    // Null Move Pruning constants
    static constexpr int NULL_MOVE_MIN_DEPTH = 3;  ///< Minimum depth for null move pruning
    static constexpr int NULL_MOVE_REDUCTION = 3;  ///< Depth reduction for null move (R value)
    
    // Reverse Futility Pruning constants
    static constexpr int RFP_DEPTH = 7;            ///< Maximum depth for reverse futility pruning
    static constexpr int RFP_MARGIN = 80;          ///< Margin per depth for reverse futility pruning
};
