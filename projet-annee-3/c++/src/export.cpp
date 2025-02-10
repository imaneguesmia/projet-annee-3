#include <nanobind/nanobind.h>

#include "logic/piece.hpp"

namespace nb = nanobind;

NB_MODULE(chess_module, m) {
    nb::enum_<PType>(m, "PType")  // Exporte l'enum PType
        .value("Pawn", PType::Pawn)
        .value("Knight", PType::Knight)
        .value("Bishop", PType::Bishop)
        .value("Rook", PType::Rook)
        .value("Queen", PType::Queen)
        .value("King", PType::King)
        .value("NoneType", PType::NoneType)
        .export_values();

    nb::class_<Piece>(m, "Piece")  // Exporte la classe Piece
        .def(nb::init<>())  // Constructeur vide
        .def(nb::init<PType, Player>())  // Constructeur avec type et joueur
        .def("getType", &Piece::getType)
        .def("getPlayer", &Piece::getPlayer)
        .def("isNone", &Piece::isNone)
        .def("fen", &Piece::fen)
        .def_static("fromFen", &Piece::fromFen);
}
