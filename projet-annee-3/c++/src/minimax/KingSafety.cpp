#include "KingSafety.hpp"
#include "MoveOrdering.hpp"
#include "../logic/attack_tables.hpp"
#include "../logic/board.hpp"
#include "../logic/position.hpp"
#include "../logic/move_generator.hpp"
#include "../logic/move.hpp"

int KingSafety::evaluate(const Board &board, Player player) const
{
    Position kingSquare = board.kingSquare(player);
    int safetyScore = 0;

    safetyScore += pawnShieldScore(board, kingSquare, player);
    safetyScore -= pawnStormPenalty(board, kingSquare, player);
    safetyScore -= kingTropismScore(board, kingSquare, player);
    safetyScore -= attackingKingZoneScore(board, kingSquare, player);
    safetyScore -= virtualMobility(board, kingSquare, player);

    return safetyScore;
}

int KingSafety::pawnShieldScore(const Board &board, Position kingSquare, Player player) const
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

int KingSafety::pawnStormPenalty(const Board &board, Position kingSquare, Player player) const
{
    int penalty = 0;
    int rank = kingSquare.getRow();

    MoveGenerator moveGen;
    std::vector<Move> moves = moveGen.generateMoves(otherPlayer(player), board, Position(), 0, false);
    for (const auto &move : moves)

    {
        if (Position(move.target).getRow() == rank + (player == Player::White ? -1 : 1))
        {
            penalty += 10;
        }
    }
    return penalty;
}

int KingSafety::kingTropismScore(const Board &board, Position kingSquare, Player player) const
{
    int score = 0;
    MoveGenerator moveGen;
    std::vector<Move> moves = moveGen.generateMoves(otherPlayer(player), board, Position(), 0, false);
    for (const auto &move : moves)
    {
        int distance = abs(kingSquare.getColumn() - Position(move.target).getColumn()) + abs(kingSquare.getRow() - Position(move.target).getRow());
        int pieceValue = (move.p_type == PType::Queen) ? 2 : 1;
        score += (distance < 3) ? 20 * pieceValue : 0;
    }
    return score;
}

int KingSafety::attackingKingZoneScore(const Board &board, Position kingSquare, Player player) const
{
    int attackScore = 0;
    int attackingPiecesCount = 0;

    MoveGenerator moveGen;
    std::vector<Move> moves = moveGen.generateMoves(otherPlayer(player), board, Position(), 0, false);
    for (const auto &move : moves)
    {
        if (abs(kingSquare.getColumn() - Position(move.target).getColumn()) <= 1 && abs(kingSquare.getRow() - Position(move.target).getRow()) <= 1)
        {
            attackingPiecesCount++;
            attackScore += attackValue(move.p_type);
        }
    }

    return attackScore * attackWeight[attackingPiecesCount] / 100;
}

int KingSafety::virtualMobility(const Board &board, Position kingSquare, Player player) const
{
    int mobilityScore = 0;

    const AttackTables &attackTables = AttackTables::getInstance();
    BB::BitBoard kingMoves = attackTables.getQueenAttackBitboard(kingSquare, board.occupancy());

    while (kingMoves)
    {
        Square sq = static_cast<Square>(BB::leastSignificantBitIndex(kingMoves));
        BoardAnalysis board_analysis;
        if (board_analysis.isSquareAttacked(sq, otherPlayer(player), board))
            mobilityScore += 5;

        BB::reset_bit(kingMoves, static_cast<int>(sq));
    }

    return mobilityScore;
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
