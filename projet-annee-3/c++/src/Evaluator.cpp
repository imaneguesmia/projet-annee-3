#include "chess/Evaluator.hpp"
#include "chess.hpp"
#include <array>

namespace chess {

uint64_t northFill(uint64_t gen){
    // https://www.chessprogramming.org/Pawn_Fills
    gen |= (gen <<  8);
    gen |= (gen << 16);
    gen |= (gen << 32);
    return gen;
}

uint64_t southFill(uint64_t gen){
    gen |= (gen >>  8);
    gen |= (gen >> 16);
    gen |= (gen >> 32);
    return gen;
}

uint64_t northOne(uint64_t bb){
    return bb << 8;
}

uint64_t southOne(uint64_t bb){
    return bb >> 8;
}

uint64_t eastOne(uint64_t b) {
    return (b << 1) & 0xFEFEFEFEFEFEFEFEULL;
}

uint64_t westOne(uint64_t b) {
    return (b >> 1) & 0x7F7F7F7F7F7F7F7FULL;
}

uint64_t wFrontSpans(uint64_t wpawns){
    uint64_t n = northFill(wpawns);
    return northOne(n);
}

uint64_t bFrontSpans(uint64_t bpawns){
    uint64_t s = southFill(bpawns);
    return southOne(s);
}

uint64_t bRearSpans (uint64_t bpawns) { 
    uint64_t n = northFill(bpawns); 
    return northOne(n);
}
uint64_t wRearSpans (uint64_t wpawns) {
    uint64_t n = southFill(wpawns); 
    return southOne (n);
}

// White attack front spans
uint64_t wEastAttackFrontSpans(uint64_t wpawns) {
    uint64_t n = wFrontSpans(wpawns);
    return eastOne(n);
}

uint64_t wWestAttackFrontSpans(uint64_t wpawns) {
    uint64_t n = wFrontSpans(wpawns);
    return westOne(n);
}

// Black attack front spans
uint64_t bEastAttackFrontSpans(uint64_t bpawns) {
    uint64_t n = bFrontSpans(bpawns);
    return eastOne(n);
}

uint64_t bWestAttackFrontSpans(uint64_t bpawns) {
    uint64_t n = bFrontSpans(bpawns);
    return westOne(n);
}

// White attack rear spans
uint64_t wEastAttackRearSpans(uint64_t wpawns) {
    // https://www.chessprogramming.org/Pawn_Fills
    uint64_t n = southFill(wpawns);
    return eastOne(n);
}

uint64_t wWestAttackRearSpans(uint64_t wpawns) {
    uint64_t n = southFill(wpawns);
    return westOne(n);
}

// Black attack rear spans
uint64_t bEastAttackRearSpans(uint64_t bpawns) {
    uint64_t n = northFill(bpawns);
    return eastOne(n);
}

uint64_t bWestAttackRearSpans(uint64_t bpawns) {
    uint64_t n = northFill(bpawns);
    return westOne(n);
}

uint64_t bPawnEastAttacks(uint64_t bpawns) {
    return eastOne(bpawns) >> 8; // Shift east, then one rank down
}

uint64_t bPawnWestAttacks(uint64_t bpawns) {
    return westOne(bpawns) >> 8; // Shift west, then one rank down
}

uint64_t wPawnEastAttacks(uint64_t wpawns) {
    return eastOne(wpawns) << 8; // Shift east, then one rank up
}

uint64_t wPawnWestAttacks(uint64_t wpawns) {
    return westOne(wpawns) << 8; // Shift west, then one rank up
}

// pawns with at least one pawn in front on the same file
uint64_t wPawnsBehindOwn(uint64_t wpawns) {
    // https://www.chessprogramming.org/Double_and_Triple_(Bitboards)
    return wpawns & wRearSpans(wpawns);
}

// Pawns with at least one pawn behind on the same file
uint64_t wPawnsInfrontOwn(uint64_t wpawns) {
    return wpawns & wFrontSpans(wpawns);
}

uint64_t bPawnsInfrontOwn(uint64_t bpawns) {
    return bpawns & bFrontSpans(bpawns);
}

uint64_t wPawnsInfrontAndBehindOwn (uint64_t wpawns) {
    return wPawnsInfrontOwn(wpawns) &  wPawnsBehindOwn(wpawns);
}
int leastSignificantBitIndex(uint64_t n) {
    // Algorithm by Kim Walisch.
    // https://www.chessprogramming.org/BitScan#KimWalisch

    static const uint64_t debruijn_hash_64 = 0x03f79d71b4cb0a89ULL;

    static const int index_64[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    // Black magic hashery
    return index_64[((n ^ (n-1)) * debruijn_hash_64) >> 58];
};

// uint64_t wStop(uint64_t wpawns) {
//     return northOne(wpawns);
// }

// uint64_t bStop(uint64_t bpawns) {
//     return southOne(bpawns);
// }


const uint64_t NOT_A_FILE = 0xfefefefefefefefe;
const uint64_t NOT_H_FILE = 0x7f7f7f7f7f7f7f7f;
const uint64_t NOT_AB_FILE = 0xfcfcfcfcfcfcfcfc;
const uint64_t NOT_GH_FILE = 0x3f3f3f3f3f3f3f3f;

uint64_t generate_knight_attacks(uint64_t knights) {
    uint64_t attack = 0;
    attack |= (knights & NOT_A_FILE) >> 15;
    attack |= (knights & NOT_H_FILE) >> 17;
    attack |= (knights & NOT_AB_FILE) >> 6;
    attack |= (knights & NOT_GH_FILE) >> 10;
    attack |= (knights & NOT_A_FILE) << 17;
    attack |= (knights & NOT_H_FILE) << 15;
    attack |= (knights & NOT_AB_FILE) << 10;
    attack |= (knights & NOT_GH_FILE) << 6;
    return attack;
}

uint64_t generate_bishop_attacks(uint64_t bishops, uint64_t occupied) {
    uint64_t attacks = 0;
    int directions[] = {9, 7, -9, -7}; // Diagonal directions
    for (int i = 0; i < 4; i++) {
        uint64_t pos = bishops;
        while (pos) {
            pos = (i < 2) ? (pos << directions[i]) : (pos >> -directions[i]);
            if (pos & occupied) break;
            attacks |= pos;
        }
    }
    return attacks;
}

uint64_t generate_rook_attacks(uint64_t rooks, uint64_t occupied) {
    uint64_t attacks = 0;
    int directions[] = {8, -8, 1, -1}; // Vertical and horizontal directions
    for (int i = 0; i < 4; i++) {
        uint64_t pos = rooks;
        while (pos) {
            pos = (i < 2) ? (pos << directions[i]) : (pos >> -directions[i]);
            if (pos & occupied) break;
            attacks |= pos;
        }
    }
    return attacks;
}

uint64_t generate_queen_attacks(uint64_t queens, uint64_t occupied) {
    return generate_bishop_attacks(queens, occupied) | generate_rook_attacks(queens, occupied);
}


// Base material values for each piece type
static constexpr int baseValues[] = {
    100,   // Pawn
    300,   // Knight
    300,   // Bishop
    500,   // Rook
    900,   // Queen
    10000, // King
    0      // None (empty square)
};

// Piece-square tables for positional evaluation
static constexpr int pieceSquareTable[7][64] =
{
    // Pawn
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        50,50,50,50,50,50,50,50,
        10,10,20,30,30,20,10,10,
        5, 5,10,25,25,10, 5, 5,
        0, 0, 0,20,20, 0, 0, 0,
        5,-5,-10, 0, 0,-10,-5, 5,
        5,10,10,-20,-20,10,10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    // Knight
    {
       -50,-40,-30,-30,-30,-30,-40,-50,
       -40,-20,  0,  0,  0,  0,-20,-40,
       -30,  0, 10, 15, 15, 10,  0,-30,
       -30,  5, 15, 20, 20, 15,  5,-30,
       -30,  0, 15, 20, 20, 15,  0,-30,
       -30,  5, 10, 15, 15, 10,  5,-30,
       -40,-20,  0,  5,  5,  0,-20,-40,
       -50,-40,-30,-30,-30,-30,-40,-50
    },
    // Bishop
    {
       -20,-10,-10,-10,-10,-10,-10,-20,
       -10,  0,  0,  0,  0,  0,  0,-10,
       -10,  0,  5, 10, 10,  5,  0,-10,
       -10,  5,  5, 10, 10,  5,  5,-10,
       -10,  0, 10, 10, 10, 10,  0,-10,
       -10, 10, 10, 10, 10, 10, 10,-10,
       -10,  5,  0,  0,  0,  0,  5,-10,
       -20,-10,-10,-10,-10,-10,-10,-20
    },
    // Rook
    {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    },
    // Queen
    {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    // King midgame
    {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    },
    // King endgame
    {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    }
};

    Evaluator::Evaluator(){
    for (int file = 0; file < 8; file++) {
        uint64_t bitboard = 0;

        for (int rank = 0; rank < 8; rank++) {
            int square = rank * 8 + file;

            if (file > 0) { // Left neighbor exists
                bitboard |= (1ULL << (square - 1));
            }
            if (file < 7) { // Right neighbor exists
                bitboard |= (1ULL << (square + 1));
            }
        }

        neighbor_files_bb[file] = bitboard;
    }
}


/**
 * @brief Mirrors a square index to reflect the board from White's perspective.
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
int Evaluator::evaluate(const Board& board)
{
    int score = 0;

    for (auto pt : {
        PieceType::PAWN,
        PieceType::KNIGHT,
        PieceType::BISHOP,
        PieceType::ROOK,
        PieceType::QUEEN,
        PieceType::KING
    }) {
        int typeIndex = static_cast<int>(pt);

        // Evaluate White pieces
        {
            Bitboard bbWhite = board.pieces(pt, Color::WHITE);
            while (bbWhite) {
                int sq = bbWhite.pop();
                score += baseValues[typeIndex];
                score += pieceSquareTable[typeIndex][sq];
            }
        }

        // Evaluate Black pieces
        {
            Bitboard bbBlack = board.pieces(pt, Color::BLACK);
            while (bbBlack) {
                int sq = bbBlack.pop();
                score -= baseValues[typeIndex];
                score -= pieceSquareTable[typeIndex][mirrorSquare(sq)];
            }
        }
    }

    // Evaluate pawn structure 
    score += pawnStructureHeuristic(board,Color::WHITE);
    score -= pawnStructureHeuristic(board,Color::BLACK);

    // Evaluate mobility
    score += mobilityHeuristic(board,Color::WHITE);
    score -= mobilityHeuristic(board,Color::BLACK);

    // Adjust score if Black is to move
    if (board.sideToMove() == Color::BLACK) {
        score = -score;
    }

    return score;
}

int Evaluator::pawnStructureHeuristic(const Board& board,Color color){
    // https://github.com/mcostalba/Stockfish/blob/master/src/pawns.cpp
    int score = 0;

    // Flags
    uint64_t passed_flag;
    uint64_t backward_pawn_stop_squares;
    uint64_t double_triple_flag;

    const uint64_t all_opp_pawn_bitboard = board.pieces(PieceType::PAWN, ~ color).getBits();
    const uint64_t pawn_bitboard = board.pieces(PieceType::PAWN, color).getBits();

    // Isolated pawns
    {
        uint64_t pawn_bitboard = board.pieces(PieceType::PAWN, color).getBits();
        while (pawn_bitboard) {
            int sq = leastSignificantBitIndex(pawn_bitboard);  // Get least significant bit index
            pawn_bitboard &= pawn_bitboard - 1;  // Clear that bit
            
            int file = sq & 7; // Get the file of the index, which corresponds to the 3-last bits
            if((neighbor_files_bb[file] & pawn_bitboard) == 0){
                score += pawn_values[file];

            }
        }
    }

    // Passed pawns
    uint64_t all_front_spans;
    if (color == Color::WHITE)
        all_front_spans = bFrontSpans(all_opp_pawn_bitboard);
    else
        all_front_spans = wFrontSpans(all_opp_pawn_bitboard);
    
    all_front_spans |= eastOne(all_front_spans)  | westOne(all_front_spans);
    passed_flag = pawn_bitboard & ~all_front_spans;

    while (passed_flag) {
        // Get the least significant bit (pawn position) 
        passed_flag &= passed_flag - 1;
        int square = leastSignificantBitIndex(passed_flag);
        
        int rank = Square(square).rank(); // Convert square to rank (0-based)

        // Index of score for this passed pawn 
        // (must flip index for black since they go in the opposite direction)
        int candidate_passed_index = (color == Color::WHITE) ? rank + 1 : 8 - rank; 

        score += candidate_passed_midgame[candidate_passed_index];
    }

    // Backward pawns
    if(color == Color::WHITE){
        uint64_t stops = northOne(pawn_bitboard);
        uint64_t wAttackSpans = wEastAttackFrontSpans(pawn_bitboard) | wWestAttackFrontSpans(pawn_bitboard);
        uint64_t bAttacks = bPawnEastAttacks(all_opp_pawn_bitboard) | bPawnWestAttacks(all_opp_pawn_bitboard);
        backward_pawn_stop_squares = stops & bAttacks & ~wAttackSpans;
    }else{
        uint64_t stops = southOne(pawn_bitboard);
        uint64_t bAttackSpans = bEastAttackFrontSpans(pawn_bitboard) | bWestAttackFrontSpans(pawn_bitboard);
        uint64_t wAttacks = wPawnEastAttacks(all_opp_pawn_bitboard) | wPawnWestAttacks(all_opp_pawn_bitboard);
        backward_pawn_stop_squares = stops & wAttacks & ~bAttackSpans;
    } 
    int count_bf = Bitboard(backward_pawn_stop_squares).count(); // Number of stop squares = number of backwards pawns
    score -= count_bf * 9; // https://github.com/mcostalba/Stockfish/blob/master/src/pawns.cpp

    // Double and triple pawns
    uint64_t doubledPawns;
    if(color == Color::WHITE)
        doubledPawns = wPawnsInfrontOwn(pawn_bitboard) ;
    else
        doubledPawns = bPawnsInfrontOwn(pawn_bitboard) ;
    int count_dp = Bitboard(doubledPawns).count();
    score -= count_dp * 12; // https://beginchess.com/2010/08/15/think-like-a-chess-engine/

    return score;
}


int Evaluator::mobilityHeuristic(const Board& board, Color color) {
    int score = 0;    
    uint64_t friendly_bb = board.us(color).getBits();
    uint64_t occupied_bb = board.occ().getBits();

    // Mobility score for Knights
    uint64_t knights_bb = board.pieces(PieceType::KNIGHT, color).getBits();
    uint64_t knights_attacks_bb = generate_knight_attacks(knights_bb) & ~friendly_bb;
    int knights_attacks_count = Bitboard(knights_attacks_bb).count() ;

    score += mobility_bonus[int(PieceType::KNIGHT)][knights_attacks_count];

    // Mobility score for Bishops
    uint64_t bishops_bb = board.pieces(PieceType::BISHOP, color).getBits();
    uint64_t bishops_attacks_bb = generate_bishop_attacks(bishops_bb,occupied_bb) & ~friendly_bb;
    int bishop_attacks_count = Bitboard(bishops_attacks_bb).count() ;

    score += mobility_bonus[int(PieceType::BISHOP)][bishop_attacks_count];

    // Mobility score for Rooks
    uint64_t rooks_bb = board.pieces(PieceType::ROOK, color).getBits();
    uint64_t rooks_attacks_bb = generate_rook_attacks(rooks_bb,occupied_bb) & ~friendly_bb;
    int rooks_attacks_count = Bitboard(rooks_attacks_bb).count() ;

    score += mobility_bonus[int(PieceType::ROOK)][rooks_attacks_count];

    // Mobility score for Queens
    uint64_t queens_bb = board.pieces(PieceType::QUEEN, color).getBits();
    uint64_t queens_attacks_bb = generate_queen_attacks(queens_bb,occupied_bb) & ~friendly_bb;
    int queens_attacks_count = Bitboard(queens_attacks_bb).count() ;

    score += mobility_bonus[int(PieceType::QUEEN)][queens_attacks_count];    
    return score;
}
} // namespace chess
