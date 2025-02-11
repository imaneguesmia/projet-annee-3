#include "chess/MoveOrdering.hpp"
#include <algorithm> // std::sort
#include "chess.hpp"

namespace chess {

MoveOrdering::MoveOrdering()
{
    // Initialiser les killerMoves
    killerMoves.resize(MAX_PLY, { Move::NO_MOVE, Move::NO_MOVE });

    // 16 types × 64 cases
    historyHeuristic.resize(16, std::vector<int>(64, 0));
}

void MoveOrdering::updateKillers(Move move, int ply)
{
    if (ply < 0 || ply >= (int)killerMoves.size()) return;

    // Si le move n'est pas déjà killer[0], on décale
    if (killerMoves[ply][0] != move) {
        killerMoves[ply][1] = killerMoves[ply][0];
        killerMoves[ply][0] = move;
    }
}

void MoveOrdering::updateHistory(const Board& board, Move move, int depth)
{
    // On incrémente le history si ce n'est pas une capture
    if (!board.isCapture(move)) {
        Piece attacker = board.at(move.from());
        int fromType   = static_cast<int>(attacker.type());
        int toIndex    = move.to().index();

        if (fromType >= 0 && fromType < (int)historyHeuristic.size()) {
            historyHeuristic[fromType][toIndex] += depth * depth;
        }
    }
}

Move MoveOrdering::killerAt(int ply, int index) const
{
    if (ply < 0 || ply >= (int)killerMoves.size()) {
        return Move::NO_MOVE;
    }
    if (index < 0 || index > 1) {
        return Move::NO_MOVE;
    }
    return killerMoves[ply][index];
}

int MoveOrdering::historyScore(const Board& board, Move move) const
{
    Piece attacker = board.at(move.from());
    int fromType   = static_cast<int>(attacker.type());
    int toIndex    = move.to().index();

    if (fromType >= 0 && fromType < (int)historyHeuristic.size()) {
        return historyHeuristic[fromType][toIndex];
    }
    return 0;
}

void MoveOrdering::orderMoves(Movelist& moves,
                              const Board& board,
                              int ply,
                              Move pvMove)
{
    std::vector<std::pair<Move,int>> scoredMoves;
    scoredMoves.reserve(moves.size());

    for (auto& mv : moves) {
        int score = 0;

        // Bonus si PV move (venant de la TT par ex.)
        if (mv == pvMove) {
            score += 100000;
        }

        // Si capture => MVV-LVA
        if (board.isCapture(mv)) {
            Piece attacker = board.at(mv.from());
            Piece victim   = board.at(mv.to());
            score += 1000
                   + static_cast<int>(victim.type()) * 10
                   - static_cast<int>(attacker.type());
        }
        else {
            // Killer moves
            if (mv == killerAt(ply, 0)) {
                score += 900;
            }
            else if (mv == killerAt(ply, 1)) {
                score += 800;
            }

            // History heuristic
            score += historyScore(board, mv);
        }

        scoredMoves.emplace_back(mv, score);
    }

    // Tri décroissant
    std::sort(scoredMoves.begin(), scoredMoves.end(),
              [](auto& a, auto& b){
                  return a.second > b.second;
              });

    moves.clear();
    for (auto& kv : scoredMoves) {
        moves.add(kv.first);
    }
}

} // namespace chess
