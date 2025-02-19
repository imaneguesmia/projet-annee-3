from ui import UIElement

from .chess_model import ChessModel

import cpp_chess as cm
from image_loader import ImageLoader

import pygame

from typing import override

class ChessBoard(UIElement):
    """UI element to display the chessboard. Essentially the View and Controller in an MVC model."""

    def __init__(self, rect: pygame.Rect, model: ChessModel):
        assert rect.width == rect.height, "ChessBoard must be square"

        super().__init__(rect)

        self.square_size = rect.width // 8
        
        # Chargement des images des pièces
        self.images = ImageLoader(self.square_size)

        self._model = model
    
    @override
    def update(self) -> None:
        pass

    @override
    def draw(self, dest: pygame.Surface) -> None:
        """Dessine le plateau avec les pièces et les coups légaux."""
        self.draw_board(dest)
                # square = cm.Position(row, col)
                # piece_on_square = self.board_view.piece_at(square)
                # coords = self.square_to_coordinates(square)

                # # Déterminer la couleur de la case
                # tile_index = (row + col) % 2

                # tile = self.images.tile_sprite(tile_index)
                # dest.blit(tile, coords)

                # if self.selected_square is not None:
                #     # Dessiner la case selectionnée
                #     if self.selected_square == square:
                #         dest.blit(self.images.selected_sprite(tile_index), coords)
                #     # Dessiner les coups légaux 
                #     else:
                #         possible_move = next((move for move in self.selected_moves if cm.Position(move.target) == square), None)

                #         if possible_move is not None:
                #             dest.blit(self.images.attacked_sprite(tile_index, possible_move.capture), coords)

                # # Dessiner la pièce sur la case
                # if piece_on_square.fen() != ".":
                #     piece_image = self.images.piece_sprite(piece_on_square)
                #     dest.blit(piece_image, coords)
    
    def draw_board(self, dest: pygame.Surface) -> None:
        """"""
        for row in range(8):
            for col in range(8):
                square = cm.Position(row, col)
                coords = self.square_to_coordinates(square)

                # Draw tiles
                tile_index = (row + col) % 2

                tile = self.images.tile_sprite(tile_index)
                dest.blit(tile, coords)

                # Draw moves
                if self._model.selected_square is not None:
                    # Draw selected square
                    if self._model.selected_square == square:
                        dest.blit(self.images.selected_sprite(tile_index), coords)
                    # Draw legal moves
                    else:
                        possible_move = next(
                            (move for move in self._model.selected_moves if cm.Position(move.target) == square),
                            None
                        )

                        if possible_move is not None:
                            dest.blit(self.images.attacked_sprite(tile_index, possible_move.capture), coords)

                # Draw pieces
                piece_on_square = self._model.piece_at(square)

                if piece_on_square.fen() != ".":
                    piece_image = self.images.piece_sprite(piece_on_square)
                    dest.blit(piece_image, coords)
    
    def coordinates_to_square(self, x: int, y: int) -> cm.Position:
        row = y // self.square_size
        col = x // self.square_size
        return cm.Position(row, col)

    def square_to_coordinates(self, square: cm.Position) -> tuple[int, int]:
        x = self.square_size * square.column
        y = self.square_size * square.row
        return x, y
    
    @override
    def on_click(self, point: tuple[int, int]) -> bool:
        self._model.select_square(self.coordinates_to_square(*point))