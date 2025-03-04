from ui import UIElement

from .chess_model import ChessModel

import image_loader as img

import cpp_chess as cm

import pygame

from typing import override

class ChessBoard(UIElement):
    """UI element to display the chessboard. Essentially the View and Controller in an MVC model."""

    def __init__(self, rect: pygame.Rect, model: ChessModel):
        assert rect.width == rect.height, "ChessBoard must be square"

        super().__init__(rect)

        self.square_size = rect.width // 8

        self._model = model

                # Stocker la position du plateau pour corriger les clics souris
        self.board_x = rect.x
        self.board_y = rect.y

    
    @override
    def update(self) -> None:
        pass

    @override
    def draw(self, dest: pygame.Surface) -> None:
        """Dessine le plateau avec les pièces et les coups légaux."""
        self.draw_board(dest)
    
    def draw_board(self, dest: pygame.Surface) -> None:
        """Draw the board with pieces and legal moves."""
        for row in range(8):
            for col in range(8):
                square = cm.Position(row, col)
                coords = self.square_to_coordinates(square)

                # Draw tiles
                tile_index = (row + col) % 2

                tile = img.IMAGES.tile_sprite(tile_index)
                dest.blit(tile, coords)

                # Draw moves
                if self._model.selected_square is not None:
                    # Draw selected square
                    if self._model.selected_square == square:
                        dest.blit(img.IMAGES.selected_sprite(tile_index), coords)
                    # Draw legal moves
                    else:
                        possible_move = next(
                            (move for move in self._model.selected_moves if cm.Position(move.target) == square),
                            None
                        )

                        if possible_move is not None:
                            dest.blit(img.IMAGES.attacked_sprite(tile_index, possible_move.capture), coords)

                # Draw pieces
                piece_on_square = self._model.piece_at(square)

                if piece_on_square.fen() != ".":
                    piece_image = img.IMAGES.piece_sprite(piece_on_square)
                    dest.blit(piece_image, coords)
    
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
    
    @override
    def on_click(self, point: tuple[int, int]) -> bool:
        square = self.coordinates_to_square(*point)

        if square is not None:  # Vérifie que le clic est bien sur le board
            self._model.select_square(square)
            return True

        return False
