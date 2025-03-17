#ifndef KING_SAFETY_HPP
#define KING_SAFETY_HPP

#include "../logic/board.hpp"

class KingSafety
{
public:
    int evaluate(const Board &board, Player player) const;

private:
    int pawnShieldScore(const Board &board, Position kingSquare, Player player) const;
    int pawnStormPenalty(const Board &board, Position kingSquare, Player player) const;
    int kingTropismScore(const Board &board, Position kingSquare, Player player) const;
    int attackingKingZoneScore(const Board &board, Position kingSquare, Player player) const;
    int virtualMobility(const Board &board, Position kingSquare, Player player) const;
    int scaleKingSafety(int safetyScore, const Board &board, Player player) const;
    int attackValue(PType pieceType) const;

    static const int attackWeight[10];
};

#endif // KING_SAFETY_HPP
