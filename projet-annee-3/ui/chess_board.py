from .ui_element import UIElement

import cpp_chess as cm
from image_loader import ImageLoader

import pygame

from typing import override

class ChessBoard(UIElement):
    def __init__(self, rect: pygame.Rect):
        assert rect.width == rect.height, "ChessBoard must be square"

        super().__init__(rect)

        self.square_size = rect.width // 8
        
        # Chargement des images des pièces
        self.images = ImageLoader(self.square_size)

        self.selected_square = None
        self.selected_moves = []  # Stocker les coups légaux de la pièce sélectionnée
    
    @override
    def update(self) -> None:
        pass

    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.fill((0, 255, 255))