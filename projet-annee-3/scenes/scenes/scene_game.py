from ..scene import Scene

from .chess_board import ChessBoard
from .chess_model import ChessModel

import cpp_chess as cm

from ui import Button, Panel
from ui.colors import *

import pygame

from typing import override

PROMOTION_PANEL_WIDTH = 400
PROMOTION_PANEL_HEIGHT = 200

PROMOTION_BUTTON_PADDING = 10
PROMOTION_BUTTON_SIZE = PROMOTION_PANEL_WIDTH/4 - PROMOTION_BUTTON_PADDING*2

class PromotionButton(Button):
    def __init__(self, position: tuple[int, int], p_type: cm.PType, model: ChessModel):
        button_rect = pygame.Rect(position, (PROMOTION_BUTTON_SIZE, PROMOTION_BUTTON_SIZE))
        piece = cm.Piece(p_type, cm.Player.White)

        super().__init__(button_rect, piece.fen())

        self._piece = piece

        self.color = (0, 0, 0, 0)
        self.hover_color = (255, 255, 255, 100)

        self._model = model
    
    @override
    def on_click(self, _) -> bool:
        print(self._piece.fen())

        self._model.do_current_promotion(self._piece.get_type())

        return True
    
    def set_player(self, player: cm.Player) -> None:
        """"""
        self._piece = cm.Piece(self._piece.get_type(), player)

        self._text.text = self._piece.fen()

class PromotionPanel(Panel):
    def __init__(self, window_rect: pygame.Rect, model: ChessModel):
        panel_rect = pygame.Rect(
            window_rect.centerx - PROMOTION_PANEL_WIDTH/2,
            window_rect.centery - PROMOTION_PANEL_HEIGHT/2, 
            PROMOTION_PANEL_WIDTH, PROMOTION_PANEL_HEIGHT
        )

        super().__init__(panel_rect, SAND_COLOR)

        self._buttons: list[PromotionButton] = []

        for i, p_type in enumerate([cm.PType.Knight, cm.PType.Bishop, cm.PType.Rook, cm.PType.Queen]):
            position = (
                PROMOTION_BUTTON_PADDING + (PROMOTION_BUTTON_SIZE+PROMOTION_BUTTON_PADDING*2)*i,
                PROMOTION_PANEL_HEIGHT/2 + PROMOTION_BUTTON_PADDING
            )
            button = PromotionButton(position, p_type, model)

            self.add_child(button)
            self._buttons.append(button)
    
    def set_player(self, player: cm.Player) -> None:
        """"""
        for b in self._buttons:
            b.set_player(player)

class scene_ChessGame(Scene, ChessModel):

    def __init__(self, window_rect: pygame.Rect, initial_state: str | None = None):
        super().__init__(window_rect)

        # self._model = ChessModel(initial_state)

        self._chess_game = cm.Game() if initial_state is None else cm.Game(initial_state)

        self.selected_square: cm.Position | None = None
        self.selected_moves: list[cm.Move] = []

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

    # -- Game model methods -- #
    
    def board_view(self) -> cm.BoardView:
        return self._chess_game.board_view()
    
    @override
    def piece_at(self, square: cm.Position) -> cm.Piece:
        return self._chess_game.get_piece_at(square)

    def select_square(self, square: cm.Position) -> None:
        """"""
        piece_on_square = self._chess_game.get_piece_at(square)
        current_player = self._chess_game.current_player

        # Update legal moves
        self._legal_moves = self._chess_game.get_current_legals()

        # Select the clicked piece if it belongs to the current player
        if not piece_on_square.is_none() and piece_on_square.get_player() == current_player:
            self.selected_square = square
            self.selected_moves = self.get_legal_moves_from_square(square)

            print(f"Selected piece : {piece_on_square.get_type()} ({piece_on_square.get_player()})") #  debug selection

        # Do a move if a piece is already selected and a valid target was clicked
        elif self.selected_square and any(cm.Position(move.target) == square for move in self.selected_moves):
            move_to_do = next((move for move in self.selected_moves if cm.Position(move.target) == square), None)

            if move_to_do:
                if move_to_do.promotion != cm.PType.NoneType:
                    self.prompt_promotion(move_to_do)
                else:
                    self._chess_game.move(move_to_do)

                    # Update data after move
                    self.selected_square = None
                    self.selected_moves = []

        else:
            self.selected_square = None
            self.selected_moves = []
    
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
            self._chess_game.move(self._promotion)

            # Update data after move
            self.selected_square = None
            self.selected_moves = []


