#include "chess/MinimaxAI.hpp"
#include "chess.hpp"

#define MATE_SCORE 10e7
namespace chess {

MinimaxAI::MinimaxAI(int depth) : searchDepth(depth) {
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

        arrNeighborFiles[file] = bitboard;
    }
}

Move MinimaxAI::getMove(Board& board) {
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        return Move::NO_MOVE;
    }

    Move bestMove = moves[0];
    int bestScore = -999999;
    int alpha = -999999;
    int beta =  999999;

    for (const Move& move : moves) {
        Board newBoard = board;
        newBoard.makeMove(move);
        int score = -minimax(newBoard, searchDepth - 1, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    return bestMove;
}

int MinimaxAI::minimax(Board& board, int depth, int alpha, int beta) {
    // Vérifier si la partie est terminée
    auto [gameResult, resultDetails] = board.isGameOver();

    // Échec et mat
    if (gameResult == GameResultReason::CHECKMATE) {
        // Si c'est un mat pour le joueur actuel, retournez une valeur négative
        // Sinon, retournez une valeur positive
        return (resultDetails == GameResult::LOSE) ? (-MATE_SCORE + depth) : (MATE_SCORE - depth);
    }

    // Partie nulle (pat, répétition, etc.)
    if (gameResult == GameResultReason::STALEMATE || 
        gameResult == GameResultReason::INSUFFICIENT_MATERIAL || 
        gameResult == GameResultReason::THREEFOLD_REPETITION) {
        return 0; // Égalité
    }

    // Si la profondeur est atteinte ou si la partie n'est pas terminée, évaluez la position
    if (depth == 0) {
        return evaluate(board);
    }

    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    int bestScore = -999999;
    for (const Move& move : moves) {
        Board newBoard = board;
        newBoard.makeMove(move);
        int score = -minimax(newBoard, depth - 1, -beta, -alpha);
        bestScore = std::max(bestScore, score);
        alpha = std::max(alpha, score);
        if (alpha >= beta) break;
    }
    return bestScore;
}

int MinimaxAI::evaluate(const Board& board) {
    static const std::unordered_map<PieceType, int, PieceType::Hash> pieceValues = {
        {PieceType::PAWN,   100},
        {PieceType::KNIGHT, 300},
        {PieceType::BISHOP, 300},
        {PieceType::ROOK,   500},
        {PieceType::QUEEN,  900},
        {PieceType::KING,   10000}
    };

    int score = 0;

    // Évaluer le matériel
    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN}) {
        score += board.pieces(pt, Color::WHITE).count() * pieceValues.at(pt);
        score -= board.pieces(pt, Color::BLACK).count() * pieceValues.at(pt);
    }

    // Malus pour éviter les répétitions
    constexpr int BACKTRACK_PENALTY = 100;

    if (board.isRepetition(2)) {
        if (board.sideToMove() == Color::WHITE) {
            score -= BACKTRACK_PENALTY;
        } else {
            score += BACKTRACK_PENALTY;
        }
    }

    // Bonus pour inciter aux échecs
    constexpr int CHECK_BONUS = 150;

    if (board.inCheck() && board.sideToMove() == Color::BLACK) {
        score += CHECK_BONUS;
    } else if (board.inCheck() && board.sideToMove() == Color::WHITE) {
        score -= CHECK_BONUS;
    }
    // Point en fonction de la position des pièces
    score += pieceSquareHeuristic(board);

    return score;
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

int MinimaxAI::pieceSquareHeuristic(const Board& board){
    int score = 0;There are many different types of pawns, but a backward pawn is one that has no support from other pawns
    auto color = board.sideToMove();

    for (auto pt : {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN,PieceType::KING}) {
        //todo Replace with (PType p_type = PType::FIRST; p_type != PType::OOB; increment_enum(p_type))
        uint64_t pieceBitboard = board.pieces(pt, color).getBits();

        while(pieceBitboard){
            int pos = leastSignificantBitIndex(pieceBitboard);
            pieceBitboard &= pieceBitboard - 1;

            // Flip for black pieces
            if (color == Color::BLACK) pos = 63 - pos;
            
            score += pieceTable[int(pt)][pos];
        }
    }

    if(color == Color::BLACK)
        score = - score;
    return score;
}

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


// uint64_t wStop(uint64_t wpawns) {
//     return northOne(wpawns);
// }

// uint64_t bStop(uint64_t bpawns) {
//     return southOne(bpawns);
// }

int MinimaxAI::pawnPatternHeuristic(const Board& board){
    int score = 0;

    // Flags
    uint64_t isolated_flag;
    uint64_t passed_flag;
    uint64_t backward_flag;
    uint64_t double_triple_flag;
    uint64_t defended_flag;

    auto color = board.sideToMove();
    uint64_t pawnBitboard = board.pieces(PieceType::PAWN, color).getBits();
    uint64_t oppPawnBitboard = board.pieces(PieceType::PAWN, ~ color).getBits();

    // Isolated pawns
    while (pawnBitboard) {
        int sq = leastSignificantBitIndex(pawnBitboard);  // Get least significant set bit
        pawnBitboard &= pawnBitboard - 1;  // Clear that bit
        
        int file = sq & 7;
        if(arrNeighborFiles[file] & pawnBitboard){
            isolated_flag = 1;
            break;
        }
    }

    // Passed pawns
    uint64_t allFrontSpans;
    if (color == Color::WHITE)
        allFrontSpans = bFrontSpans(pawnBitboard);
    else
        allFrontSpans = wFrontSpans(pawnBitboard);
    
    allFrontSpans |= eastOne(allFrontSpans)  | westOne(allFrontSpans);
    passed_flag = pawnBitboard & ~allFrontSpans;

    // Backward pawns
    uint64_t stops = pawnBitboard << 8;
    uint64_t AttackSpans = wEastAttackFrontSpans(pawnBitboard) | wWestAttackFrontSpans(pawnBitboard);
    if(color == Color::WHITE){
        uint64_t bAttacks = bPawnEastAttacks(oppPawnBitboard) | bPawnWestAttacks(oppPawnBitboard);
        backward_flag = (stops & bAttacks & ~AttackSpans) >> 8;
    }else{
        uint64_t wAttacks = wPawnEastAttacks(oppPawnBitboard) | wPawnWestAttacks(oppPawnBitboard);
        backward_flag = (stops & wAttacks & ~AttackSpans) >> 8;
    }

    // Double and triple pawns

    // Defended pawns

    // Scores according to flags 

    return score;
}
}
