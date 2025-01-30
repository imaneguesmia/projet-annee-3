#pragma once

#include "player.hpp"

// enum class Piece {
//     W_Pawn = 0b0000, W_Knight, W_Bishop,
//     W_Rook, W_Queen, W_King,

//     B_Pawn = 0b1000, B_Knight, B_Bishop,
//     B_Rook, B_Queen, B_King,

//     None
// };

class Piece {

public:
    enum Id {
        W_Pawn, W_Knight, W_Bishop,
        W_Rook, W_Queen, W_King,

        B_Pawn, B_Knight, B_Bishop,
        B_Rook, B_Queen, B_King,

        NoneId = -1
    };

    enum Type {
        Pawn, Knight, Bishop,
        Rook, Queen, King,

        NoneType = -1
    };

private:
    const Id id;

    const Type type;
    const Player player;

    static Type idType(Id id) {
        return id == Id::NoneId ? Type::NoneType : Type(id % 6);
    };
    static Player idPlayer(Id id) { return id >= 6 ? Player::Black : Player::White; };

    static Id idFromDetails(Type type, Player player) {
        return type == Type::NoneType ? Id::NoneId : Id(type + (6 * int(player))); 
    }

    static Id fenToId(char fen);

public:
    Piece(char fen)
        : id(fenToId(fen))
        , type(idType(id))
        , player(idPlayer(id))
    {};
    Piece(Id id)
        : id(id)
        , type(idType(id))
        , player(idPlayer(id))
    {};
    Piece(Type type, Player player)
        : id(idFromDetails(type, player))
        , type(type)
        , player(player)
    {};
    Piece() : Piece(Id(-1)) {};
    ~Piece() {};

    Id getId() const { return id; };
    Type getType() const { return type; };

    bool isNone() const { return id == Id::NoneId; };

    char fen() const;

    Player getPlayer() const { return player; };
};