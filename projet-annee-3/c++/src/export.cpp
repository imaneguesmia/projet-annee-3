#include "logic/player.hpp"
#include "logic/position.hpp"
#include "logic/piece.hpp"
#include "logic/move.hpp"
#include "logic/game_data.hpp"

#include "game_management/move_prompter.hpp"
#include "game_management/game_manager.hpp"
#include "game_management/move_provider.hpp"

#include "minimax/evaluator_settings.hpp"

#include "view/board_view.hpp"

#include <nanobind/nanobind.h>

#include <nanobind/stl/string.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/unique_ptr.h>

#include <sstream>

namespace nb = nanobind;

void export_player(nb::module_& m) {
    nb::enum_<Player>(m, "Player")
        .value("White", Player::White)
        .value("Black", Player::Black);
    
    m.def("other_player", &otherPlayer);
}

void export_position(nb::module_& m) {
    nb::enum_<Square>(m, "Square")
        .value("a8", Square::a8) .value("b8", Square::b8) .value("c8", Square::c8) .value("d8", Square::d8) .value("e8", Square::e8) .value("f8", Square::f8) .value("g8", Square::g8) .value("h8", Square::h8)
        .value("a7", Square::a7) .value("b7", Square::b7) .value("c7", Square::c7) .value("d7", Square::d7) .value("e7", Square::e7) .value("f7", Square::f7) .value("g7", Square::g7) .value("h7", Square::h7)
        .value("a6", Square::a6) .value("b6", Square::b6) .value("c6", Square::c6) .value("d6", Square::d6) .value("e6", Square::e6) .value("f6", Square::f6) .value("g6", Square::g6) .value("h6", Square::h6)
        .value("a5", Square::a5) .value("b5", Square::b5) .value("c5", Square::c5) .value("d5", Square::d5) .value("e5", Square::e5) .value("f5", Square::f5) .value("g5", Square::g5) .value("h5", Square::h5)
        .value("a4", Square::a4) .value("b4", Square::b4) .value("c4", Square::c4) .value("d4", Square::d4) .value("e4", Square::e4) .value("f4", Square::f4) .value("g4", Square::g4) .value("h4", Square::h4)
        .value("a3", Square::a3) .value("b3", Square::b3) .value("c3", Square::c3) .value("d3", Square::d3) .value("e3", Square::e3) .value("f3", Square::f3) .value("g3", Square::g3) .value("h3", Square::h3)
        .value("a1", Square::a1) .value("b1", Square::b1) .value("c1", Square::c1) .value("d1", Square::d1) .value("e1", Square::e1) .value("f1", Square::f1) .value("g1", Square::g1) .value("h1", Square::h1)
        .value("Invalid", Square::Invalid);
    
    nb::class_<Position>(m, "Position")
        .def(nb::init<int, int>())
        .def(nb::init<Square>())
        .def(nb::init<int>())
        .def(nb::init<const std::string&>())
        .def(nb::init<>())

        .def("set_position_invalid", &Position::setPositionInvalid)
        .def_prop_ro("is_valid", &Position::isValid)

        .def_prop_ro("row", &Position::getRow)
        .def_prop_ro("column", &Position::getColumn)

        .def_prop_rw("position_square", &Position::getPositionSquare, nb::overload_cast<Square>(&Position::setPositionSquare))

        .def_prop_rw("position_string", &Position::getPositionString, &Position::setPositionString)

        .def("__repr__", &Position::getPositionString)
        .def("__eq__", [](const Position &a, const Position &b) { return a == b; });


}

void export_piece(nb::module_& m) {
    nb::enum_<PType>(m, "PType")  // Exporte l'enum PType
        .value("Pawn", PType::Pawn)
        .value("Knight", PType::Knight)
        .value("Bishop", PType::Bishop)
        .value("Rook", PType::Rook)
        .value("Queen", PType::Queen)
        .value("King", PType::King)
        .value("NoneType", PType::NoneType);

    nb::class_<Piece>(m, "Piece")  // Exporte la classe Piece
        .def(nb::init<>())  // Constructeur vide
        .def(nb::init<PType, Player>())  // Constructeur avec type et joueur
        .def("get_type", &Piece::getType)
        .def("get_player", &Piece::getPlayer)
        .def("is_none", &Piece::isNone)
        .def("fen", &Piece::fen)
        .def_static("from_fen", &Piece::fromFen);
}

void export_move(nb::module_& m) {
    nb::class_<Move>(m, "Move")
        .def(nb::init<>())

        .def_rw("source", &Move::source)
        .def_rw("target", &Move::target)
        .def_rw("player", &Move::player)
        .def_rw("p_type", &Move::p_type)
        .def_rw("promotion", &Move::promotion)
        .def_rw("capture", &Move::capture)
        .def_rw("double_push", &Move::double_push)
        .def_rw("en_passant", &Move::en_passant)
        .def_rw("castle", &Move::castle)

        .def("__repr__", [](Move& self) {
            std::ostringstream out;
            out << self;
            return out.str();
        });
}

void export_board_view(nb::module_& m) {
    nb::class_<BoardView>(m, "BoardView")
        .def("piece_at", &BoardView::pieceAt);
}

void export_game(nb::module_& m) {
    nb::enum_<GameState>(m, "GameState")
        .value("INGAME", GameState::INGAME)
        .value("CHECKMATE", GameState::CHECKMATE)
        .value("STALEMATE", GameState::STALEMATE)
        .value("HALF_MOVE_DRAW", GameState::HALF_MOVE_DRAW)
        .value("REPETITION", GameState::REPETITION);
    
    nb::class_<GameData>(m, "GameData")
        .def_prop_ro("current_player", &GameData::getCurrentPlayer)
        .def_prop_ro("castling_rights", &GameData::getCastlingRights)
        .def_prop_ro("en_passant_position", &GameData::getEnPassantPosition)
        
        .def("get_current_pseudo_legals", &GameData::getCurrentPseudoLegals)
        .def("get_current_legals", &GameData::getCurrentLegals)

        .def_prop_ro("is_currently_in_check", &GameData::isCurrentlyInCheck)
        .def_prop_ro("game_state", &GameData::getGameState)

        .def("get_piece_at", &GameData::getPieceAt);

    nb::class_<ExtendedGameData>(m, "ExtendedGameData");
    
    nb::class_<MovePrompter>(m, "MovePrompter")
        .def("game_data", &MovePrompter::gameData, nb::rv_policy::reference)
        .def("extended_game_data", &MovePrompter::extendedGameData, nb::rv_policy::reference)

        .def("propose_move", &MovePrompter::proposeMove);
    
    nb::enum_<EvaluatorType>(m, "EvaluatorType")
        .value("Standard", EvaluatorType::Standard)
        .value("NNUE", EvaluatorType::NNUE);

    nb::class_<EvaluatorSettings>(m, "EvaluatorSettings")
        .def(nb::init<>())

        .def_rw("type", &EvaluatorSettings::type)

        .def_rw("depth", &EvaluatorSettings::depth)

        .def_rw("material", &EvaluatorSettings::material)
        .def_rw("piece_square_table", &EvaluatorSettings::pieceSquareTable)
        .def_rw("mobility", &EvaluatorSettings::mobility)
        .def_rw("pawn_structure", &EvaluatorSettings::pawnStructure)
        .def_rw("king_safety", &EvaluatorSettings::kingSafety)

        .def_rw("type", &EvaluatorSettings::type)
        .def_rw("network_path", &EvaluatorSettings::networkPath)
        .def_rw("nnue_host", &EvaluatorSettings::nnueHost)
        .def_rw("nnue_port", &EvaluatorSettings::nnuePort);
    
    nb::class_<GameManager>(m, "GameManager")
        .def(nb::init<>())
        .def(nb::init<const std::string&>())

        .def_prop_ro("current_player", &GameManager::getCurrentPlayer)

        .def("game_data", &GameManager::gameData, nb::rv_policy::reference)
        .def("extended_game_data", &GameManager::extendedGameData, nb::rv_policy::reference)

        .def("create_ai_player", &GameManager::createAIPlayer)
    
        .def("make_move", &GameManager::makeMove);
}

void export_ai(nb::module_& m) {
    nb::class_<AIMoveProvider>(m, "AIMoveProvider")
        .def("get_move", &AIMoveProvider::getMove)
        .def("get_position_value", &AIMoveProvider::getPositionValue);
}

NB_MODULE(chess_module, m) {
    export_player(m);
    export_position(m);
    export_piece(m);
    export_move(m);
    export_board_view(m);
    export_game(m);
    export_ai(m);
}
