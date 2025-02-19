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