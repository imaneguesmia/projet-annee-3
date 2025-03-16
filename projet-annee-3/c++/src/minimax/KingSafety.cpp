#include "KingSafety.hpp"
#include "MoveOrdering.hpp"
#include "../logic/attack_tables.hpp"
#include "../../external/chess-library/chess.hpp"

int KingSafety::evaluate(const Board &board, Player player) const
{
    Square kingSquare = board.kingSquare(player);
    int safetyScore = 0;

    safetyScore += pawnShieldScore(board, kingSquare, player);
    safetyScore -= pawnStormPenalty(board, kingSquare, player);
    safetyScore -= kingTropismScore(board, kingSquare, player);
    safetyScore -= attackingKingZoneScore(board, kingSquare, player);
    safetyScore -= virtualMobility(board, kingSquare, player);

    return scaleKingSafety(safetyScore, board, player);
}

int KingSafety::pawnShieldScore(const Board &board, Square kingSquare, Player player) const
{
    int score = 0;
    int rank = kingSquare.getRow();

    if ((player == Player::White && rank >= 6) || (player == Player::Black && rank <= 1))
    {
        score += 20;
    }
    else
    {
        score -= 30;
    }
    return score;
}

int KingSafety::pawnStormPenalty(const Board &board, Square kingSquare, Player player) const
{
    int penalty = 0;
    int rank = kingSquare.getRow();
    for (const auto &move : board.legalMoves(otherPlayer(player)))
    {
        if (move.to().getRow() == rank + (player == Player::White ? -1 : 1))
        {
            penalty += 10;
        }
    }
    return penalty;
}

int KingSafety::kingTropismScore(const Board &board, Square kingSquare, Player player) const
{
    int score = 0;
    for (const auto &move : board.legalMoves(otherPlayer(player)))
    {
        int distance = abs(kingSquare.getCol() - move.to().getCol()) + abs(kingSquare.getRow() - move.to().getRow());
        int pieceValue = (move.piece().getType() == PType::Queen) ? 2 : 1;
        score += (distance < 3) ? 20 * pieceValue : 0;
    }
    return score;
}

int KingSafety::attackingKingZoneScore(const Board &board, Square kingSquare, Player player) const
{
    int attackScore = 0;
    int attackingPiecesCount = 0;

    for (const auto &move : board.legalMoves(otherPlayer(player)))
    {
        if (abs(kingSquare.getCol() - move.to().getCol()) <= 1 && abs(kingSquare.getRow() - move.to().getRow()) <= 1)
        {
            attackingPiecesCount++;
            attackScore += attackValue(move.piece().getType());
        }
    }

    return attackScore * attackWeight[attackingPiecesCount] / 100;
}

int KingSafety::virtualMobility(const Board &board, Square kingSquare, Player player) const
{
    int mobilityScore = 0;
    BB::BitBoard kingMoves = AttackTables::getQueenAttackBitboard(kingSquare, board.occupancy());

    while (kingMoves)
    {
        Square sq = BB::leastSignificantBitIndex(kingMoves);
        if (board.isAttacked(sq, otherPlayer(player)))
            mobilityScore += 5;

        BB::reset_bit(kingMoves, sq);
    }

    return mobilityScore;
}

int KingSafety::scaleKingSafety(int safetyScore, const Board &board, Player player) const
{
    int materialWeight = board.materialScore(player);
    return (safetyScore * materialWeight) / 100;
}

int KingSafety::attackValue(PType pieceType) const
{
    switch (pieceType)
    {
    case PType::Knight:
        return 2;
    case PType::Bishop:
        return 2;
    case PType::Rook:
        return 3;
    case PType::Queen:
        return 5;
    default:
        return 0;
    }
}

const int KingSafety::attackWeight[10] = {
    0, 0, 50, 75, 88, 94, 97, 99, 99, 100};
