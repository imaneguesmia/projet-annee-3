#pragma once

#include "player.hpp"

/**
 * @brief Class representing a chess piece.
 */
class Piece {

public:
    // Enum over piece ids.
    enum Id {
        W_Pawn, W_Knight, W_Bishop,
        W_Rook, W_Queen, W_King,

        B_Pawn, B_Knight, B_Bishop,
        B_Rook, B_Queen, B_King,

        NoneId    // Special value indicating no piece.
    };

    // Enum over piece type, i.e. pieces without player information.
    enum Type {
        Pawn, Knight, Bishop,
        Rook, Queen, King,

        NoneType  // Special value indicating no piece.
    };

private:
    const Id id;

    const Type type;
    const Player player;

    // Gets the type of a piece with the given id.
    static Type idType(Id id) {
        return id == Id::NoneId ? Type::NoneType : Type(id % 6);
    };
    // Gets the player of a piece with the given id.
    static Player idPlayer(Id id) { return id >= 6 ? Player::Black : Player::White; };

    // Gets the id of a piece with the given type and player.
    static Id idFromDetails(Type type, Player player) {
        return type == Type::NoneType ? Id::NoneId : Id(type + (6 * int(player))); 
    }

    // Gets the id of a piece with the given FEN symbol.
    static Id fenToId(char fen);

public:
    // Constructs a new Piece object with the given id.
    Piece(const Id id)
        : id(id)
        , type(idType(id))
        , player(idPlayer(id))
    {};
    // Constructs a new Piece object with the given type and player.
    Piece(const Type type, const Player player)
        : id(idFromDetails(type, player))
        , type(type)
        , player(player)
    {};
    // Constructs an empty Piece object.
    Piece() : Piece(Id::NoneId) {};
    ~Piece() {};

    Id getId() const { return id; };

    Type getType() const { return type; };
    Player getPlayer() const { return player; };

    // Returns `true` if the piece is empty.
    bool isNone() const { return id == Id::NoneId; };

    // Returns the FEN symbol of the piece.
    char fen() const;

    // Builds and returns a Piece object with the given FEN symbol.
    static Piece fromFen(char fen) { return Piece(fenToId(fen)); };
};