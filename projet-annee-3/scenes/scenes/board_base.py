from ui import UIElement

import cpp_chess as cm

import pygame

from abc import ABC

class BoardBase(UIElement, ABC):
    """Base of all UI elements with the chessboard coordinate system."""

    def __init__(self, rect: pygame.Rect):
        assert rect.width == rect.height, "ChessBoard must be square"

        super().__init__(rect)

        self.square_size = rect.width // 8

        # Stocker la position du plateau pour corriger les clics souris
        self.board_x = rect.x
        self.board_y = rect.y
    
    def coordinates_to_square(self, x: int, y: int) -> cm.Position:
        """Convertit les coordonnées de la souris en case d'échecs."""
        
        # Ajuster pour la position du board centré
        x -= self.board_x
        y -= self.board_y

        # Vérifier si le clic est en dehors du plateau
        if x < 0 or x >= self.square_size * 8 or y < 0 or y >= self.square_size * 8:
            return None  # Clic en dehors de l’échiquier

        row = y // self.square_size
        col = x // self.square_size
        return cm.Position(row, col)

    def square_to_coordinates(self, square: cm.Position) -> tuple[int, int]:
        x = self.square_size * square.column
        y = self.square_size * square.row
        return x, y
    
