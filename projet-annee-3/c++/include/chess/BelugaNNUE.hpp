#ifndef CHESS_ENGINE_BELUGANNUE_HPP
#define CHESS_ENGINE_BELUGANNUE_HPP

#include <cstdint>
#include "Player.hpp"
#include "TranspositionTable.hpp"
#include "NNUEEvaluator.hpp"
#include "MoveOrdering.hpp"
#include "QuiescenceSearchNNUE.hpp"

namespace chess {

/**
 * @class BelugaNNUE
 * @brief Implements the NegaMax search with alpha-beta pruning, aspiration windows, and NNUE evaluation.
 * This version uses a neural network for position evaluation instead of traditional handcrafted evaluation.
 */
class BelugaNNUE : public Player {
public:
    /**
     * @brief Constructor for the BelugaNNUE search engine.
     * @param networkPath Path to the NNUE network file.
     * Initializes with a default search depth of 4.
     */
    explicit BelugaNNUE(const std::string& networkPath);

    ~BelugaNNUE() override = default;

    /**
     * @brief Determines the best move for the current board position.
     * @param board The current chess board state.
     * @return The best move found.
     */
    Move getMove(Board& board) override;

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
    int negamax(Board& board, int depth, int alpha, int beta, int ply);

    /**
     * @brief Evaluates a terminal game state (checkmate, stalemate, etc.).
     * @param reason The reason for game termination.
     * @param result The result of the game (win/loss/draw).
     * @param ply Current depth in the search.
     * @return Evaluation score based on the game result.
     */
    int evaluateTerminal(GameResultReason reason, GameResult result, int ply) const;

private:
    int searchDepth;                 ///< Maximum search depth.
    TranspositionTable transpositionTable; ///< Transposition table for caching results.
    MoveOrdering moveOrdering;       ///< Move ordering heuristics.
    NNUEEvaluator evaluator;         ///< Neural network evaluator.

    static constexpr int INF = 100000000;   ///< Representation of infinity for search values.
    static constexpr int MATE_SCORE = 1000000; ///< Score for a checkmate position.
    static constexpr int ASP_WIN = 50;      ///< Aspiration window value.
};

} // namespace chess

#endif // CHESS_ENGINE_BELUGANNUE_HPP 