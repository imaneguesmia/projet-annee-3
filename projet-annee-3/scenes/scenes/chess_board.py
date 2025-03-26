from .board_base import BoardBase

from .chess_board_callback_interface import ChessBoardCallbackInterface

import image_loader as img

import cpp_chess as cm

import pygame

from typing import override

class ChessBoard(BoardBase):
    """UI element to display the chessboard. Essentially the View and Controller in an MVC model."""

    def __init__(self, rect: pygame.Rect, callbacks: ChessBoardCallbackInterface):
        super().__init__(rect)

        self._callbacks = callbacks

        self.is_game_over = False  # If `True` handle click events differently
    
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
                if self._callbacks.selected_square is not None:
                    # Draw selected square
                    if self._callbacks.selected_square == square:
                        dest.blit(img.IMAGES.selected_sprite(tile_index), coords)
                    # Draw legal moves
                    else:
                        possible_move = next(
                            (move for move in self._callbacks.selected_moves if cm.Position(move.target) == square),
                            None
                        )

                        if possible_move is not None:
                            dest.blit(img.IMAGES.attacked_sprite(tile_index, possible_move.capture), coords)

                # Draw pieces
                piece_on_square = self._callbacks.piece_at(square)

                if piece_on_square.fen() != ".":
                    piece_image = img.IMAGES.piece_sprite(piece_on_square)
                    dest.blit(piece_image, coords)
    
    @override
    def on_click(self, point: tuple[int, int]) -> bool:
        if self.is_game_over:
            self._callbacks.on_click_after_game_end()

            return True
        else:
            square = self.coordinates_to_square(*point)

            if square is not None:  # Vérifie que le clic est bien sur le board
                self._callbacks.select_square(square)
                return True

            return False
