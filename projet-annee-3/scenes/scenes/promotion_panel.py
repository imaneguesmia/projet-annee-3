from .chess_board_callback_interface import ChessBoardCallbackInterface

import image_loader as img

from ui import Button, NinepatchPanel, Image
from ui.colors import *

import cpp_chess as cm

import pygame

from typing import override

PANEL_WIDTH = 400
PANEL_HEIGHT = 100
PANEL_PADDING = 8

BUTTON_PADDING = 8
BUTTON_SIZE = PANEL_WIDTH/4 - BUTTON_PADDING*2

class PromotionButton(Button):
    def __init__(self, position: tuple[int, int], p_type: cm.PType, model: ChessBoardCallbackInterface):
        button_rect = pygame.Rect(position, (BUTTON_SIZE, BUTTON_SIZE))
        piece = cm.Piece(p_type, cm.Player.White)

        super().__init__(button_rect, "")

        self._piece = piece
        self._model = model

        self.color = (0, 0, 0, 0)
        self.hover_color = (255, 255, 255, 100)

        image_rect = button_rect.copy()
        image_rect.topleft = (0, 0)
        self._image = Image(image_rect, img.IMAGES.piece_sprite(self._piece))

        self.add_child(self._image)
    
    @override
    def on_click(self, _) -> bool:
        print(self._piece.fen())

        self._model.do_current_promotion(self._piece.get_type())

        return True
    
    def set_player(self, player: cm.Player) -> None:
        """"""
        self._piece = cm.Piece(self._piece.get_type(), player)

        self._image.image = img.IMAGES.piece_sprite(self._piece)

        # self._text.text = self._piece.fen()

class PromotionPanel(NinepatchPanel):
    def __init__(self, board_rect: pygame.Rect, model: ChessBoardCallbackInterface):
        panel_rect = pygame.Rect(
            board_rect.centerx - PANEL_WIDTH/2 - PANEL_PADDING,
            board_rect.centery - PANEL_HEIGHT/2 - PANEL_PADDING, 
            PANEL_WIDTH + PANEL_PADDING*2,
            PANEL_HEIGHT + PANEL_PADDING*2
        )

        super().__init__(panel_rect, img.IMAGES.panel(img.PanelTheme.TEST), 31)

        self._buttons: list[PromotionButton] = []

        for i, p_type in enumerate([cm.PType.Knight, cm.PType.Bishop, cm.PType.Rook, cm.PType.Queen]):
            position = (
                PANEL_PADDING + BUTTON_PADDING + (BUTTON_SIZE+BUTTON_PADDING*2)*i,
                PANEL_PADDING + BUTTON_PADDING
            )
            button = PromotionButton(position, p_type, model)

            self.add_child(button)
            self._buttons.append(button)
    
    def set_player(self, player: cm.Player) -> None:
        """"""
        for b in self._buttons:
            b.set_player(player)