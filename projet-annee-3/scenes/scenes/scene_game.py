from ..scene import Scene

from ..data_transfer.player_type import PlayerType

from .chess_board import ChessBoard
from .chess_model import ChessModel

from .promotion_panel import PromotionPanel


import cpp_chess as cm

from ui.colors import *

import pygame

from typing import override

class scene_ChessGame(Scene, ChessModel):

    def __init__(self, 
        window_rect: pygame.Rect, 
        initial_state: str | None = None,
        white_player: PlayerType = PlayerType.HUMAN,
        black_player: PlayerType = PlayerType.HUMAN
    ):
        super().__init__(window_rect)

        self._chess_game = cm.GameManager() if initial_state is None else cm.GameManager(initial_state)
        # self._move_prompter: cm.MovePrompter = None
        self._game_data: cm.GameData = None

        self._selected_square: cm.Position | None = None
        self._selected_moves: list[cm.Move] = []

        self._promotion: cm.Move | None = None

        self._board = ChessBoard(
            pygame.Rect(0, 0, window_rect.height, window_rect.height),
            self
        )

        self._promotion_panel = PromotionPanel(window_rect, self)
        self._promotion_panel.is_visible = False
        self._promotion_panel.can_accept_events = False

        self.elements.append(self._board)
        self.elements.append(self._promotion_panel)

        # Initialize AI engines

        self._player_types = [white_player, black_player]

        if PlayerType.MINIMAX in self._player_types:
            self._minimax_engine: cm.AIMoveProvider = self._chess_game.create_minimax_player(10)
        if PlayerType.NEURAL_NET in self._player_types:
            self._neural_net_engine: cm.AIMoveProvider = ...
        
        self.game_turn()
        # print(self._chess_game.game_data().get_current_legals(False))
    
    @property
    def selected_square(self) -> cm.Position | None: 
        return self._selected_square
    
    @property
    def selected_moves(self) -> list[cm.Move]:
        return self._selected_moves
    
    # -- Game loop -- #

    def get_player_type(self, player: cm.Player) -> PlayerType:
        """"""
        return self._player_types[0] if player == cm.Player.White else self._player_types[1]

    def game_turn(self) -> None:
        """"""
        self._game_data = self._chess_game.game_data()

        current_player = self._chess_game.current_player
        current_player_type = self.get_player_type(current_player)

        match current_player_type:
            case PlayerType.HUMAN:
                # Allow the player to make a move
                self._board.can_accept_events = True
                pass
            case PlayerType.MINIMAX:
                # Prevent human from moving
                self._board.can_accept_events = False

                move_to_make = self._minimax_engine.get_move(
                    self._chess_game.extended_game_data()
                )
                self.make_move(move_to_make)

                pass
            case PlayerType.NEURAL_NET:
                # Prevent human from moving
                self._board.can_accept_events = False
                pass

    # -- Game model methods -- #
    
    # def board_view(self) -> cm.BoardView:
    #     return self._chess_game.board_view()

    def make_move(self, move: cm.Move) -> None:
        """"""
        success = self._chess_game.make_move(move)

        if success:
            print("made move")
            print(move)

            # Update data after move
            self._selected_square = None
            self._selected_moves = []

            # Next turn
            self.game_turn()
        else:
            print("INVALID MOVE")
            exit()
    
    @override
    def piece_at(self, square: cm.Position) -> cm.Piece:
        return self._game_data.get_piece_at(square)

    def select_square(self, square: cm.Position) -> None:
        """"""
        piece_on_square = self._game_data.get_piece_at(square)
        current_player = self._game_data.current_player

        # Update legal moves
        self._legal_moves = self._game_data.get_current_legals(False)

        # Select the clicked piece if it belongs to the current player
        if not piece_on_square.is_none() and piece_on_square.get_player() == current_player:
            self._selected_square = square
            self._selected_moves = self.get_legal_moves_from_square(square)

            print(f"Selected piece : {piece_on_square.get_type()} ({piece_on_square.get_player()})") #  debug selection

        # Do a move if a piece is already selected and a valid target was clicked
        elif self._selected_square and any(cm.Position(move.target) == square for move in self._selected_moves):
            move_to_do = next((move for move in self._selected_moves if cm.Position(move.target) == square), None)

            if move_to_do:
                if move_to_do.promotion != cm.PType.NoneType:
                    self.prompt_promotion(move_to_do)
                else:
                    self.make_move(move_to_do)

        else:
            self._selected_square = None
            self._selected_moves = []
    
    def get_legal_moves_from_square(self, square: cm.Position) -> list[cm.Move]:
        filtered_moves = []

        for move in self._legal_moves:
            move_position = cm.Position(move.source)

            if move_position == square:
                filtered_moves.append(move)

        return filtered_moves

    @override
    def prompt_promotion(self, move: cm.Move) -> None:
        self._promotion = move

        self._promotion_panel.set_player(move.player)

        self._promotion_panel.is_visible = True
        self._promotion_panel.can_accept_events = True

        self._board.can_accept_events = False
    
    @override
    def do_current_promotion(self, to_piece: cm.PType) -> None:
        if self._promotion is not None:
            self._promotion_panel.is_visible = False
            self._promotion_panel.can_accept_events = False

            self._board.can_accept_events = True

            self._promotion.promotion = to_piece
            self.make_move(self._promotion)


