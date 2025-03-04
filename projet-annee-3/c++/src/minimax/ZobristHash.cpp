#include "ZobristHash.hpp"

#include "../logic/game.hpp"

#include "../misc/increment_enum.hpp"
#include "../misc/random64.hpp"

#include <random>

/* ---- DEFINE class ZobristHash ---- */

void ZobristHash::generateKeys() {
    for (Square position = Square::FIRST; position != Square::OOB; increment_enum(position)) {
        for (Player player = Player::FIRST; player != Player::OOB; increment_enum(player)) {
            for (PType piece = PType::FIRST; piece != PType::OOB; increment_enum(piece)) {
                table[position][player][piece] = random64();
            }
        }
    }

    // black_to_move = random64();

}

ZobristHash::ZobristHash()
    : black_to_move(random64())
    , castling({
        random64(), random64(), random64(), random64(), random64(), random64(), random64(), random64(),
        random64(), random64(), random64(), random64(), random64(), random64(), random64(), random64()
    })
    , en_passant_column({
        random64(), random64(), random64(), random64(), random64(), random64(), random64(), random64()
    })
{
    generateKeys();
}

uint64_t ZobristHash::hash(const Game& game) const {
    uint64_t h = 0;

    // Player

    if (game.getCurrentPlayer() == Player::Black) {
        h ^= black_to_move;
    }

    // Pieces

    for (Square position = Square::FIRST; position != Square::OOB; increment_enum(position)) {
        Piece piece_at = game.getPieceAt(position);

        if (!piece_at.isNone()) {
            h ^= table[position][piece_at.getPlayer()][piece_at.getType()];
        }
    }

    // Castling rights

    h ^= castling[game.getCastlingRights()];

    // En passant column

    Position en_passant_pos = game.getEnPassantPosition();

    if (en_passant_pos.isValid()) {
        h ^= en_passant_column[en_passant_pos.getColumn()];
    }

    return h;
}

/* ---- END DEFINE ---- */