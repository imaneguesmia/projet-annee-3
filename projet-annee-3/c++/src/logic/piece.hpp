#pragma once

#include "player.hpp"

// Enum over piece type, i.e. pieces without player information.
enum class PType {
    Pawn, Knight, Bishop,
    Rook, Queen, King,

    NoneType,  // Special value indicating no piece.

    OOB, FIRST = Pawn, LAST = King,
};

/**
 * @brief Class representing a chess piece.
 */
class Piece {

private:
    PType type;
    Player player;

public:
    // Constructs a new Piece object with the given type and player.
    Piece(const PType type, const Player player)
        : type(type)
        , player(player)
    {};
    // Constructs an empty Piece object.
    Piece() : Piece(PType::NoneType, Player::White) {};
    ~Piece() {};

    PType getType() const { return type; };
    Player getPlayer() const { return player; };

    // Returns `true` if the piece is empty.
    bool isNone() const { return type == PType::NoneType; };

    // Returns the FEN symbol of the piece.
    char fen() const;

    // Builds and returns a Piece object with the given FEN symbol.
    static Piece fromFen(char fen);
};