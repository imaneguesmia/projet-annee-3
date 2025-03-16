#include "Evaluator.hpp"

#include "eval_utils.hpp"

#include "../logic/bitboard.hpp"
#include "../logic/board.hpp"
#include "../logic/position.hpp"
#include "../logic/player.hpp"
#include "../logic/piece.hpp"
#include "KingSafety.hpp"

#include <array>
#include <cstdint>

Evaluator::Evaluator(std::shared_ptr<const AttackTables> at)
    : at(std::move(at))
{
    for (int file = 0; file < 8; file++)
    {
        uint64_t bitboard = 0;

        for (int rank = 0; rank < 8; rank++)
        {
            int square = rank * 8 + file;

            if (file > 0)
            { // Left neighbor exists
                bitboard |= (1ULL << (square - 1));
            }
            if (file < 7)
            { // Right neighbor exists
                bitboard |= (1ULL << (square + 1));
            }
        }

        arrNeighborFiles[file] = bitboard;
    };
};

/**
 * @brief
 * @param sq Square index (0-63).
 * @return Mirrored square index.
 */
int Evaluator::mirrorSquare(int sq) const
{
    return sq ^ 56; // Flips the rank for Black's perspective
}

/**
 * @brief Evaluates the given board position.
 * @param board The current chess board state.
 * @return The evaluation score from the current's Player perspective.
 */
int Evaluator::evaluate(const Board &board, Player player)
{
    int score = 0;

    for (auto pt : {
             PType::Pawn,
             PType::Knight,
             PType::Bishop,
             PType::Rook,
             PType::Queen,
             PType::King})
    {
        int typeIndex = static_cast<int>(pt);

        // Evaluate White pieces
        {
            Piece piece{pt, Player::White};
            BB::BitBoard bbWhite = board.bitboard(piece);
            while (bbWhite)
            {
                int sq = BB::leastSignificantBitIndex(bbWhite);
                bbWhite &= bbWhite - 1;
                score += baseValues[typeIndex];
                score += pieceSquareTable[typeIndex][sq];
            }
        }

        // Evaluate Black pieces
        {
            Piece piece{pt, Player::Black};
            BB::BitBoard bbBlack = board.bitboard(piece);
            while (bbBlack)
            {
                int sq = BB::leastSignificantBitIndex(bbBlack);
                bbBlack &= bbBlack - 1;
                score -= baseValues[typeIndex];
                score -= pieceSquareTable[typeIndex][mirrorSquare(sq)];
            }
        }
    }

    // Evaluate pawn structure
    score += pawnStructureHeuristic(board, Player::White);
    score -= pawnStructureHeuristic(board, Player::Black);

    // Adjust score if Black is to move
    if (player == Player::Black)
    { // todo change board ig
        score = -score;
    }

    // KingSafety

    KingSafety kingSafety;
    score += kingSafety.evaluate(board, player);

    return score;
}

uint64_t northFill(uint64_t gen)
{
    // https://www.chessprogramming.org/Pawn_Fills
    gen |= (gen << 8);
    gen |= (gen << 16);
    gen |= (gen << 32);
    return gen;
}

uint64_t southFill(uint64_t gen)
{
    gen |= (gen >> 8);
    gen |= (gen >> 16);
    gen |= (gen >> 32);
    return gen;
}

uint64_t northOne(uint64_t bb)
{
    return bb << 8;
}

uint64_t southOne(uint64_t bb)
{
    return bb >> 8;
}

uint64_t eastOne(uint64_t b)
{
    return (b << 1) & 0xFEFEFEFEFEFEFEFEULL;
}

uint64_t westOne(uint64_t b)
{
    return (b >> 1) & 0x7F7F7F7F7F7F7F7FULL;
}

uint64_t wFrontSpans(uint64_t wpawns)
{
    uint64_t n = northFill(wpawns);
    return northOne(n);
}

uint64_t bFrontSpans(uint64_t bpawns)
{
    uint64_t s = southFill(bpawns);
    return southOne(s);
}

uint64_t bRearSpans(uint64_t bpawns)
{
    uint64_t n = northFill(bpawns);
    return northOne(n);
}
uint64_t wRearSpans(uint64_t wpawns)
{
    uint64_t n = southFill(wpawns);
    return southOne(n);
}

// White attack front spans
uint64_t wEastAttackFrontSpans(uint64_t wpawns)
{
    uint64_t n = wFrontSpans(wpawns);
    return eastOne(n);
}

uint64_t wWestAttackFrontSpans(uint64_t wpawns)
{
    uint64_t n = wFrontSpans(wpawns);
    return westOne(n);
}

// Black attack front spans
uint64_t bEastAttackFrontSpans(uint64_t bpawns)
{
    uint64_t n = bFrontSpans(bpawns);
    return eastOne(n);
}

uint64_t bWestAttackFrontSpans(uint64_t bpawns)
{
    uint64_t n = bFrontSpans(bpawns);
    return westOne(n);
}

// White attack rear spans
uint64_t wEastAttackRearSpans(uint64_t wpawns)
{
    // https://www.chessprogramming.org/Pawn_Fills
    uint64_t n = southFill(wpawns);
    return eastOne(n);
}

uint64_t wWestAttackRearSpans(uint64_t wpawns)
{
    uint64_t n = southFill(wpawns);
    return westOne(n);
}

// Black attack rear spans
uint64_t bEastAttackRearSpans(uint64_t bpawns)
{
    uint64_t n = northFill(bpawns);
    return eastOne(n);
}

uint64_t bWestAttackRearSpans(uint64_t bpawns)
{
    uint64_t n = northFill(bpawns);
    return westOne(n);
}

uint64_t bPawnEastAttacks(uint64_t bpawns)
{
    return eastOne(bpawns) >> 8; // Shift east, then one rank down
}

uint64_t bPawnWestAttacks(uint64_t bpawns)
{
    return westOne(bpawns) >> 8; // Shift west, then one rank down
}

uint64_t wPawnEastAttacks(uint64_t wpawns)
{
    return eastOne(wpawns) << 8; // Shift east, then one rank up
}

uint64_t wPawnWestAttacks(uint64_t wpawns)
{
    return westOne(wpawns) << 8; // Shift west, then one rank up
}

// pawns with at least one pawn in front on the same file
uint64_t wPawnsBehindOwn(uint64_t wpawns)
{
    // https://www.chessprogramming.org/Double_and_Triple_(Bitboards)
    return wpawns & wRearSpans(wpawns);
}

// Pawns with at least one pawn behind on the same file
uint64_t wPawnsInfrontOwn(uint64_t wpawns)
{
    return wpawns & wFrontSpans(wpawns);
}

// Pawns with at least one pawn behind on the same file
uint64_t bPawnsInfrontOwn(uint64_t bpawns)
{
    return bpawns & bFrontSpans(bpawns);
}

uint64_t wPawnsInfrontAndBehindOwn(uint64_t wpawns)
{
    return wPawnsInfrontOwn(wpawns) & wPawnsBehindOwn(wpawns);
}

int Evaluator::isolatedPawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard)
{
    int score = 0;
    uint64_t isolated_flag;

    while (pawnBitboard)
    {
        int sq = BB::leastSignificantBitIndex(pawnBitboard); // Get least significant bit index
        pawnBitboard &= pawnBitboard - 1;                    // Clear that bit

        int file = sq & 7; // Get the file of the index, which corresponds to the 3-last bits
        if ((arrNeighborFiles[file] & pawnBitboard) == 0)
        {
            isolated_flag = 1;
            score += pawnValues[file];
        }
    }
    return score;
}

int Evaluator::passedPawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard)
{
    int score = 0;

    uint64_t allFrontSpans;
    if (player == Player::White)
        allFrontSpans = bFrontSpans(pawnBitboard);
    else
        allFrontSpans = wFrontSpans(pawnBitboard);

    allFrontSpans |= eastOne(allFrontSpans) | westOne(allFrontSpans);
    uint64_t passed_flag = pawnBitboard & ~allFrontSpans;

    while (passed_flag)
    {
        int square = BB::leastSignificantBitIndex(passed_flag); // Get the least significant bit (pawn position)
        passed_flag &= passed_flag - 1;

        int rank = Position(square).getRow(); // Convert square to rank (0-based)

        // Index of score for this passed pawn
        // (must flip index for black since they go in the opposite direction)
        int candidate_passed_index = (player == Player::White) ? rank + 1 : 8 - rank;

        score += candidatePassedMidgame[candidate_passed_index];
    }

    return score;
}

int Evaluator::backwardPawnHeuristic(const Board &board, Player player, BB::BitBoard pawn_bitboard, BB::BitBoard all_opp_pawn_bitboard)
{
    int score = 0;

    uint64_t backward_pawn_stop_squares;

    if (player == Player::White)
    {
        uint64_t stops = northOne(pawn_bitboard);
        uint64_t wAttackSpans = wEastAttackFrontSpans(pawn_bitboard) | wWestAttackFrontSpans(pawn_bitboard);
        uint64_t bAttacks = bPawnEastAttacks(all_opp_pawn_bitboard) | bPawnWestAttacks(all_opp_pawn_bitboard);
        backward_pawn_stop_squares = stops & bAttacks & ~wAttackSpans;
    }
    else
    {
        uint64_t stops = southOne(pawn_bitboard);
        uint64_t bAttackSpans = bEastAttackFrontSpans(pawn_bitboard) | bWestAttackFrontSpans(pawn_bitboard);
        uint64_t wAttacks = wPawnEastAttacks(all_opp_pawn_bitboard) | wPawnWestAttacks(all_opp_pawn_bitboard);
        backward_pawn_stop_squares = stops & wAttacks & ~bAttackSpans;
    }
    int count_bf = BB::popcount(backward_pawn_stop_squares); // Number of stop squares = number of backwards pawns
    score -= count_bf * 9;                                   // https://github.com/mcostalba/Stockfish/blob/master/src/pawns.cpp

    return score;
}

int Evaluator::doubleTriplePawnHeuristic(const Board &board, Player player, BB::BitBoard pawnBitboard)
{
    uint64_t doubledPawns = (player == Player::White) ? wPawnsInfrontOwn(pawnBitboard) : bPawnsInfrontOwn(pawnBitboard);
    return -BB::popcount(doubledPawns) * 12; // https://beginchess.com/2010/08/15/think-like-a-chess-engine/
}

int Evaluator::pawnStructureHeuristic(const Board &board, Player player)
{
    // https://github.com/mcostalba/Stockfish/blob/master/src/pawns.cpp
    int score = 0;

    Piece piece{PType::Pawn, player};
    BB::BitBoard pawnBitboard = board.bitboard(piece);

    Piece opposite_piece{PType::Pawn, otherPlayer(player)};
    BB::BitBoard oppPawnBitboard = board.bitboard(opposite_piece);

    score += isolatedPawnHeuristic(board, player, pawnBitboard);
    score += passedPawnHeuristic(board, player, pawnBitboard);
    score += backwardPawnHeuristic(board, player, pawnBitboard, oppPawnBitboard);
    score += doubleTriplePawnHeuristic(board, player, pawnBitboard);
    return score;
}

int Evaluator::mobilityHeuristic(const Board &board, Player player)
{
    int score = 0;
    BB::BitBoard friendly_bb = board.occupancy(player);
    BB::BitBoard occupied_bb = board.occupancy(otherPlayer(player));

    // Mobility score for Knights
    Piece knight{PType::Knight, player};
    BB::BitBoard knights_bb = board.bitboard(knight);
    BB::BitBoard knights_attacks_bb = at->generateSetwiseKnightAttacks(knights_bb) & ~friendly_bb;
    int knights_attacks_count = BB::popcount(knights_attacks_bb);

    score += mobility_bonus[int(PType::Knight)][knights_attacks_count];

    // Mobility score for Bishops
    Piece bishop{PType::Bishop, player};
    BB::BitBoard bishops_bb = board.bitboard(bishop);
    BB::BitBoard bishops_attacks_bb = at->generateSetwiseBishopAttacks(bishops_bb, occupied_bb) & ~friendly_bb;
    int bishop_attacks_count = BB::popcount(bishops_attacks_bb);

    score += mobility_bonus[int(PType::Bishop)][bishop_attacks_count];

    // Mobility score for Rooks
    Piece rook{PType::Rook, player};
    BB::BitBoard rooks_bb = board.bitboard(rook);
    BB::BitBoard rooks_attacks_bb = at->generateSetwiseRookAttacks(rooks_bb, occupied_bb) & ~friendly_bb;
    int rooks_attacks_count = BB::popcount(rooks_attacks_bb);

    score += mobility_bonus[int(PType::Rook)][rooks_attacks_count];

    // Mobility score for Queens
    Piece queen{PType::Queen, player};
    BB::BitBoard queens_bb = board.bitboard(queen);
    BB::BitBoard queens_attacks_bb = at->generateSetwiseQueenAttacks(queens_bb, occupied_bb) & ~friendly_bb;
    int queens_attacks_count = BB::popcount(queens_attacks_bb);

    score += mobility_bonus[int(PType::Queen)][queens_attacks_count];
    return score;
}