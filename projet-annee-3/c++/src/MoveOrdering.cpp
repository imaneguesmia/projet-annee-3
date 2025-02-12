#include "chess/MoveOrdering.hpp"
#include <algorithm> // std::sort
#include "chess.hpp"

namespace chess {

MoveOrdering::MoveOrdering()
{
}

void MoveOrdering::updateKillers(Move move, int ply)
{
    if (ply < 0 || ply >= MAX_PLY)  return;

    // Shift existing killer move if it's different
    if (killerMoves[ply][0] != move) {
        killerMoves[ply][1] = killerMoves[ply][0];
        killerMoves[ply][0] = move;
    }
}

    void MoveOrdering::updateHistory(const Board& board, Move move, int depth)
{
    if (!board.isCapture(move)) {
        int from = move.from().index();
        int to = move.to().index();
        Color side = board.sideToMove();
        int updateValue = depth * (depth + 1) / 2; ///< more stable than depth * depth
        historyHeuristic[side][from][to] += updateValue;
        // Capping
        if (historyHeuristic[side][from][to] > HISTORY_MAX) {
            historyHeuristic[side][from][to] = HISTORY_MAX;
        }
    }
}

void MoveOrdering::decayHistory()
{
    for (int s = 0; s < 2; ++s) {
        for (int f = 0; f < 64; ++f) {
            for (int t = 0; t < 64; ++t) {
                historyHeuristic[s][f][t] /= 2; // Slowly decreases old values
            }
        }
    }
}

Move MoveOrdering::killerAt(int ply, int index) const
{
    if  (ply < 0 || ply >= MAX_PLY || index < 0 || index > 1) {
        return Move::NO_MOVE;
    }
    return killerMoves[ply][index];
}

int MoveOrdering::historyScore(const Board& board, Move move) const
{
    int from = move.from().index();
    int to = move.to().index();
    Color side = board.sideToMove();

    // Normalize to prevent history from overpowering other heuristics
    return historyHeuristic[side][from][to] / 32;
}

void MoveOrdering::orderMoves(Movelist& moves, const Board& board, int ply, Move pvMove)
{
    std::vector<std::pair<Move, int>> scoredMoves;
    scoredMoves.reserve(moves.size());

    for (auto& mv : moves) {
        int score = 0;

//         Assign a high score to PV move
        if (mv == pvMove) {
            score += 100000;
        }

        // Capture move scoring using MVV-LVA heuristic
        if (board.isCapture(mv)) {
            Piece attacker = board.at(mv.from());
            Piece victim = board.at(mv.to());
            score += 1000 + (int)victim.type() * 10 - (int)attacker.type();
        }
        // Quiet move
        else {
            // Assign bonuses for killer moves
            if (mv == killerAt(ply, 0)) {
                score += 900;
            }
            else if (mv == killerAt(ply, 1)) {
                score += 800;
            }

            // Add history heuristic score
            score += historyScore(board, mv);
        }

        scoredMoves.emplace_back(mv, score);
    }

    // Sort moves in descending order of score
    std::sort(scoredMoves.begin(), scoredMoves.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    // Clear original moves and repopulate with sorted moves
    moves.clear();
    for (const auto& kv : scoredMoves) {
        moves.add(kv.first);
    }
}

} // namespace chess
