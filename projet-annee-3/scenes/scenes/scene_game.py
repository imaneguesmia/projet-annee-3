from ..scene import Scene

from .chess_board import ChessBoard
from .chess_model import ChessModel

import cpp_chess as cm

import pygame

class scene_ChessGame(Scene, ChessModel):
    def __init__(self, window_rect: pygame.Rect, initial_state: str | None = None):
        super().__init__(window_rect)

        # self._model = ChessModel(initial_state)

        self._chess_game = cm.Game() if initial_state is None else cm.Game(initial_state)

        self.selected_square: cm.Position | None = None
        self.selected_moves: list[cm.Move] = []

        self._board = ChessBoard(
            pygame.Rect(0, 0, window_rect.height, window_rect.height),
            self
        )

        self.elements.append(self._board)

    # -- Game model methods -- #
    
    def board_view(self) -> cm.BoardView:
        return self._chess_game.board_view()
    
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
                self._chess_game.move(move_to_do)
                print("Moved the piece")

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
