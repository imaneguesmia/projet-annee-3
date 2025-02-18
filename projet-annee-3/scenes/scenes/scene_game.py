from ..scene import Scene

from ui import ChessBoard

import cpp_chess as cm

import pygame

def board_onclick(self: ChessBoard, point: tuple[int, int]) -> bool:
    print("board clicked at", point)

class scene_ChessGame(Scene):
    def __init__(self, window_rect: pygame.Rect, initial_state: str | None = None):
        super().__init__(window_rect)

        self._chess_game = cm.Game() if initial_state is None else cm.Game(initial_state)
        self._legal_moves = self._chess_game.get_current_pseudo_legals()

        self._board = ChessBoard(pygame.Rect(0, 0, window_rect.height, window_rect.height))
        self._board.on_click = board_onclick.__get__(self._board)

        self.elements.append(self._board)
