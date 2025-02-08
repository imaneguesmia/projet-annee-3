#pragma once

#include "player.hpp"

/**
 * @brief Class representing a chess piece.
 */
class Piece {

public:
    // Enum over piece type, i.e. pieces without player information.
    enum Type {
        Pawn, Knight, Bishop,
        Rook, Queen, King,

        NoneType  // Special value indicating no piece.
    };

private:
    Type type;
    Player player;

public:
    // Constructs a new Piece object with the given type and player.
    Piece(const Type type, const Player player)
        : type(type)
        , player(player)
    {};
    // Constructs an empty Piece object.
    Piece() : Piece(Type::NoneType, Player::White) {};
    ~Piece() {};

    Type getType() const { return type; };
    Player getPlayer() const { return player; };

    // Returns `true` if the piece is empty.
    bool isNone() const { return type == Type::NoneType; };

    // Returns the FEN symbol of the piece.
    char fen() const;

    // Builds and returns a Piece object with the given FEN symbol.
    static Piece fromFen(char fen);
};