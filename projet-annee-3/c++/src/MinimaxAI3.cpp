#include "chess/MinimaxAI3.hpp"
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <limits>

/*
Search optimisations done(may be improved):
    Iterative deapening
    Aspiration windows
    Quiescence search
    Transposition table
    Move ordering:
        -MVV-LVA
        -Killer Move
        -PV-Move
        -History heuristic

 */
namespace chess {

static constexpr int INF       = 100000000;  // "infinite" bound for alpha/beta
static constexpr int MATE_SCORE= 1000000;    // checkmate scoring base
static constexpr int ASP_WIN   = 50;         // aspiration window half‐width

// Bound Types for Transposition Table
enum class Bound { EXACT, LOWER, UPPER };

struct TTEntry {
    int          score;
    int          depth;
    Bound        bound;
    Move         bestMove;
};

MinimaxAI3::MinimaxAI3(int depth)
    : searchDepth(depth)
{
    // typical max ply for killer moves
    killerMoves.resize(64, std::array<Move,2>{Move::NO_MOVE, Move::NO_MOVE});

    // 16 piece types × 64 squares for history
    historyHeuristic.resize(16, std::vector<int>(64, 0));
}


Move MinimaxAI3::getMove(Board& board) {
    /*
      This function does iterative deepening from 1..searchDepth,
      using a small aspiration window around the best score found
      so far. If a fail-high/low occurs, it re-searches with the full window.
    */
    Move bestMove   = Move::NO_MOVE;
    int  bestScore  = -INF;

    // On the very first iteration, we have no “previous bestScore,”
    // so start with the widest window.
    int alphaGlobal = -INF;
    int betaGlobal  = +INF;

    for (int currentDepth = 1; currentDepth <= searchDepth; ++currentDepth) {
        // Use an aspiration window around bestScore only after the first iteration
        int alpha = (currentDepth == 1) ? alphaGlobal : bestScore - ASP_WIN;
        int beta  = (currentDepth == 1) ? betaGlobal  : bestScore + ASP_WIN;

        int score = negamax(board, currentDepth, alpha, beta, /*ply=*/0);

        // If we had a fail‐high or fail‐low, re‐search with full window
        if (score <= alpha || score >= beta) {
            score = negamax(board, currentDepth, -INF, +INF, /*ply=*/0);
        }

        bestScore = score;
        auto it = transpositionTable.find(board.hash());
        if (it != transpositionTable.end()) {
            bestMove = it->second.bestMove;
        }
    }

    return bestMove;
}


int MinimaxAI3::negamax(Board& board, int depth, int alpha, int beta, int ply) {
    /*
      Pure negamax with alpha–beta.
    */
    const std::uint64_t zKey = board.hash();
    auto ttIt = transpositionTable.find(zKey);

    if (ply) {
        // prevent draw in winning positions
        if (board.isRepetition(1) || board.isHalfMoveDraw()) return 0;}

    // Transposition table lookup
    if (ttIt != transpositionTable.end()) {
        const TTEntry &entry = ttIt->second;
        if (entry.depth >= depth) {
            // If the stored bound is EXACT, or within alpha/beta, we can return
            if (entry.bound == Bound::EXACT) {
                return entry.score;
            }
            // Otherwise, we can use it to narrow alpha or beta
            if (entry.bound == Bound::LOWER && entry.score > alpha) {
                alpha = entry.score;
            } else if (entry.bound == Bound::UPPER && entry.score < beta) {
                beta = entry.score;
            }
            if (alpha >= beta) {
                return entry.score;
            }
        }
    }

    // Terminal check (checkmate / stalemate / draw)
    auto [gameResultReason, gameResult] = board.isGameOver();
    if (gameResultReason != GameResultReason::NONE) {
        // Evaluate terminal position
        return evaluateTerminal(gameResultReason, gameResult, ply);
    }

    // Reached maximum depth => use quiescence
    if (depth <= 0) {
        return quiescence(board, alpha, beta, ply);
    }

    // Normal move search
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        return evaluate(board);
    }

    // Order moves: prefer captures, killers, PV moves, etc.
    Move ttBestMove = Move::NO_MOVE;
    if (ttIt != transpositionTable.end()) {
        ttBestMove = ttIt->second.bestMove;
    }
    orderMoves(moves, board, ply, ttBestMove);

    int bestValue   = -INF;
    int alphaOrig   = alpha;
    Move bestMove   = Move::NO_MOVE;

    for (size_t i = 0; i < moves.size(); ++i) {
        const Move move = moves[i];

        board.makeMove(move);
        int val = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        board.unmakeMove(move);

        if (val > bestValue) {
            bestValue = val;
            bestMove  = move;
        }
        if (bestValue > alpha) {
            alpha = bestValue;
        }
        if (alpha >= beta) {
            // Record killer/history moves
            updateKillers(move, ply);
            // Optionally update history heuristic if not a capture
            if (!board.isCapture(move)) {
                const int fromType = static_cast<int>(board.at(move.from()).type());
                historyHeuristic[fromType][move.to().index()] += depth * depth;
            }
            break; // alpha–beta cutoff
        }
    }

    // Store in TT
    Bound bound;
    if (bestValue <= alphaOrig) {
        // Fail‐low => upper bound
        bound = Bound::UPPER;
    } else if (bestValue >= beta) {
        // Fail‐high => lower bound
        bound = Bound::LOWER;
    } else {
        // Otherwise exact
        bound = Bound::EXACT;
    }

    TTEntry newEntry{bestValue, depth, bound, bestMove};
    transpositionTable[zKey] = newEntry;

    return bestValue;
}


int MinimaxAI3::quiescence(Board& board, int alpha, int beta, int ply) {
    /*
      Quiescence search (negamax style).
      Evaluate the position (“stand pat”), then explore only captures (and possibly checks)
      to avoid horizon effects.
    */
    int standPat = evaluate(board);

    if (standPat >= beta) {
        return beta;
    }
    if (standPat > alpha) {
        alpha = standPat;
    }

    Movelist captures;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(captures, board);

    orderMoves(captures, board, ply, Move::NO_MOVE);

    for (const auto& capture : captures) {
        board.makeMove(capture);
        int score = -quiescence(board, -beta, -alpha, ply + 1);
        board.unmakeMove(capture);

        if (score > standPat) {
            standPat = score;
            if (score > alpha) {
                alpha = score;
            }
            if (alpha >= beta) {
                break;
            }
        }
    }

    return alpha;
}


void MinimaxAI3::orderMoves(Movelist& moves, Board& board, int ply, Move pvMove) {
    /*
      typical move ordering strategy:
       - Give a big bonus if it’s the principal variation move (from TT).
       - Give bonuses for captures (MVV-LVA).
       - Give bonuses for killer moves.
       - Give some history heuristic bonus if not capture.
      Then sort descending by these “scores.”
    */
    std::vector<std::pair<Move, int>> scoredMoves;
    scoredMoves.reserve(moves.size());

    for (auto& mv : moves) {
        int score = 0;

        // Principal Variation from TT
        if (mv == pvMove) {
            score += 100000;
        }

        // If capture, MVV-LVA (roughly victim minus attacker)
        if (board.isCapture(mv)) {
            const Piece attacker = board.at(mv.from());
            const Piece victim   = board.at(mv.to());
            score += 1000
                  + static_cast<int>(victim.type()) * 10
                  - static_cast<int>(attacker.type());
        }
        else {
            // Killer move?
            if (mv == killerMoves[ply][0]) score += 900;
            if (mv == killerMoves[ply][1]) score += 800;

            // History heuristic?
            int fromType = static_cast<int>(board.at(mv.from()).type());
            score += historyHeuristic[fromType][mv.to().index()];
        }

        scoredMoves.emplace_back(mv, score);
    }

    std::sort(scoredMoves.begin(), scoredMoves.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second; // descending
              });

    moves.clear();
    for (auto& kv : scoredMoves) {
        moves.add(kv.first);
    }
}

// -----------------------------------------------------------------------------

void MinimaxAI3::updateKillers(Move move, int ply) {
    // If this move is different from the first killer,
    // shift them down and store it in [0].
    if (killerMoves[ply][0] != move) {
        killerMoves[ply][1] = killerMoves[ply][0];
        killerMoves[ply][0] = move;
    }
}

// -----------------------------------------------------------------------------

int MinimaxAI3::evaluateTerminal(GameResultReason reason,
                                 GameResult result,
                                 int ply) const
{
    /*
      If it’s checkmate, then from the side‐to‐move’s perspective:
        - “WIN” means side‐to‐move has delivered checkmate => big positive
        - “LOSE” means side‐to‐move is checkmated => big negative
      The `(MATE_SCORE - ply)` part helps the engine find mate quickly.
    */
    if (reason == GameResultReason::CHECKMATE) {
        if (result == GameResult::WIN) {
            return  MATE_SCORE - ply;
        } else {
            return -MATE_SCORE + ply;
        }
    }
    // Otherwise (draw / stalemate / etc.)
    return 0;
}

// -----------------------------------------------------------------------------

int MinimaxAI3::evaluate(const Board& board) {
    /*
      Returns a score from the current side‐to‐move's perspective.
      White’s total material minus Black’s total material if White to move,
      or the inverse if Black to move.
    */
    static const std::unordered_map<PieceType,int, PieceType::Hash> pieceValues = {
        {PieceType::PAWN,   100},
        {PieceType::KNIGHT, 300},
        {PieceType::BISHOP, 300},
        {PieceType::ROOK,   500},
        {PieceType::QUEEN,  900},
        {PieceType::KING,   10000}
    };

    int baseScore = 0;
    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP,
                    PieceType::ROOK, PieceType::QUEEN})
    {
        baseScore += board.pieces(pt, Color::WHITE).count() * pieceValues.at(pt);
        baseScore -= board.pieces(pt, Color::BLACK).count() * pieceValues.at(pt);
    }


    if (board.sideToMove() == Color::BLACK) {
        baseScore = -baseScore;
    }

    return baseScore;
}

} // namespace chess



