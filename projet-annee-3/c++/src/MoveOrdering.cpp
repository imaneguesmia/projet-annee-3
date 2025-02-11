#include "chess/MoveOrdering.hpp"
#include <algorithm> // std::sort
#include "chess.hpp"

namespace chess {

MoveOrdering::MoveOrdering()
{
    // Initialize killer moves with NO_MOVE
    killerMoves.resize(MAX_PLY, { Move::NO_MOVE, Move::NO_MOVE });

    // Initialize history heuristic for 16 piece types × 64 squares
    historyHeuristic.resize(16, std::vector<int>(64, 0));
}

void MoveOrdering::updateKillers(Move move, int ply)
{
    if (ply < 0 || ply >= static_cast<int>(killerMoves.size())) return;

    // Shift existing killer move if it's different
    if (killerMoves[ply][0] != move) {
        killerMoves[ply][1] = killerMoves[ply][0];
        killerMoves[ply][0] = move;
    }
}

void MoveOrdering::updateHistory(const Board& board, Move move, int depth)
{
    // Update history only if the move is not a capture
    if (!board.isCapture(move)) {
        Piece attacker = board.at(move.from());
        int fromType = static_cast<int>(attacker.type());
        int toIndex = move.to().index();

        if (fromType >= 0 && fromType < static_cast<int>(historyHeuristic.size())) {
            historyHeuristic[fromType][toIndex] += depth * depth;
        }
    }
}

Move MoveOrdering::killerAt(int ply, int index) const
{
    if (ply < 0 || ply >= static_cast<int>(killerMoves.size()) || index < 0 || index > 1) {
        return Move::NO_MOVE;
    }
    return killerMoves[ply][index];
}

int MoveOrdering::historyScore(const Board& board, Move move) const
{
    Piece attacker = board.at(move.from());
    int fromType = static_cast<int>(attacker.type());
    int toIndex = move.to().index();

    if (fromType >= 0 && fromType < static_cast<int>(historyHeuristic.size())) {
        return historyHeuristic[fromType][toIndex];
    }
    return 0;
}

void MoveOrdering::orderMoves(Movelist& moves, const Board& board, int ply, Move pvMove)
{
    std::vector<std::pair<Move, int>> scoredMoves;
    scoredMoves.reserve(moves.size());

    for (auto& mv : moves) {
        int score = 0;

        // Assign a high score to PV move
        if (mv == pvMove) {
            score += 100000;
        }

        // Capture move scoring using MVV-LVA heuristic
        if (board.isCapture(mv)) {
            Piece attacker = board.at(mv.from());
            Piece victim = board.at(mv.to());
            score += 1000 + static_cast<int>(victim.type()) * 10 - static_cast<int>(attacker.type());
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
