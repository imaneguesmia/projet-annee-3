from ..scene import Scene

from ..data_transfer import PlayerType, PlayerInfo

from .puzzles import PuzzleInfo

from .chess_board import ChessBoard
from .chess_board_callback_interface import ChessBoardCallbackInterface

from .promotion_panel import PromotionPanel
from .game_end_panel import GameEndPanel
from .side_bar import SideBar, PANEL_WIDTH as SIDEBAR_WIDTH

import cpp_chess as cm

from ui import Button
from ui.font import FONT_PATH, FONT_SIZE_MEDIUM
from ui.colors import *

from ..scene_change import SceneId

import pygame

from typing import override
from abc import ABC, abstractmethod

# ---- UI constants ---- #

BOARD_SIZE = 800

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

# ---- DEFINE class scene_ChessGame ---- #

class AbstractChessScene(Scene, ChessBoardCallbackInterface, ABC):

    def __init__(self, 
        window_rect: pygame.Rect, 
        initial_state: str | None
    ):
        super().__init__(window_rect)

        self._initial_state = initial_state

        self._chess_game = cm.GameManager(initial_state) if initial_state is not None else cm.GameManager()
        self._game_data: cm.GameData = None
        self._extended_game_data: cm.ExtendedGameData = None

        self._selected_square: cm.Position | None = None
        self._selected_moves: list[cm.Move] = []

        self._promotion: cm.Move | None = None

        # Centrer l'échiquier de 800x800 dans la fenêtre 1920x1080
        self.board_x = (window_rect.width - SIDEBAR_WIDTH) // 2 - BOARD_SIZE // 2
        self.board_y = (window_rect.height - BOARD_SIZE) // 2

        self._board = ChessBoard(
            pygame.Rect(self.board_x, self.board_y, BOARD_SIZE, BOARD_SIZE),
            self
        )
        self.elements.append(self._board)

        self._side_bar = self._create_side_panel(window_rect)
        self.elements.append(self._side_bar)
    
    @property
    def selected_square(self) -> cm.Position | None: 
        return self._selected_square
    
    @property
    def selected_moves(self) -> list[cm.Move]:
        return self._selected_moves

    # -- Game model methods -- #
    
    def update_stored_game_data(self) -> None:
        """Updates the cached game data."""
        self._game_data = self._chess_game.game_data()
        self._extended_game_data = self._chess_game.extended_game_data()
    
    @override
    def piece_at(self, square: cm.Position) -> cm.Piece:
        return self._game_data.get_piece_at(square)
    
    @abstractmethod
    def on_move_chosen(self, move: cm.Move) -> None:
        """Do something with the move chosen by the human player."""

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
                self.on_move_chosen(move_to_do)

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
    
    # -- UI elements -- #

    @abstractmethod
    def _create_side_panel(self, window_rect: pygame.Rect) -> SideBar:
        """"""

# ---- END DEFINE ---- #

