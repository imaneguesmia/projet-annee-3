from .abstract_chess_scene import AbstractChessScene

from .puzzles import PuzzleInfo, ValidMoveInfo, MoveTuple

from .puzzle_side_bar import PuzzleSideBar, PANEL_WIDTH as SIDEBAR_WIDTH

import cpp_chess as cm

from ui import Button
from ui.font import FONT_PATH, FONT_SIZE_MEDIUM
from ui.colors import *

import pygame

from typing import override

# ---- UI constants ---- #

BOARD_SIZE = 800

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

# ---- DEFINE class scene_Puzzle ---- #

class scene_Puzzle(AbstractChessScene):

    def __init__(self, 
        window_rect: pygame.Rect, 
        puzzle_info: PuzzleInfo
    ):
        self._info = puzzle_info

        super().__init__(window_rect, puzzle_info.initial_state)

        self._current_result_info: ValidMoveInfo = None   # The result info being displayed. None for initial state.
        self._current_result_position = -1                # The result position currently being shown. -1 for initial state.

        self.update_stored_game_data()

        self._side_bar: PuzzleSideBar

        old = self._board.on_click

        def new_on_click(_) -> bool:
            self._side_bar.clear_wrong()

            return old(_)
        
        self._board.on_click = new_on_click
    
    @property
    def selected_square(self) -> cm.Position | None: 
        return self._selected_square
    
    @property
    def selected_moves(self) -> list[cm.Move]:
        return self._selected_moves

    # -- Puzzle methods -- #

    @override
    def on_move_chosen(self, move):
        move_tuple = (move.source, move.target)

        valid_move_info = self._info.valid_moves.get(move_tuple, None)

        if valid_move_info is not None:
            self._chess_game.make_move(move)

            self._selected_square = None
            self._selected_moves = []

            self.present_solution_result(valid_move_info)
        else:
            self._side_bar.indicate_wrong()
    
    def present_solution_result(self, valid_move_info: ValidMoveInfo) -> None:
        """Switches mode to present the given solution result."""
        self._board.is_game_over = True

        self._current_result_info = valid_move_info
        self._current_result_position = 0

        self._side_bar.indicate_correct()
        self.next_result_position()
    
    def next_result_position(self) -> None:
        """Go to next result position."""
        result_position = self._current_result_position

        descriptions = self._current_result_info.description
        result_path = self._current_result_info.result
        highlights = self._current_result_info.highlights
        
        if result_position < len(descriptions):
            old_desc = self._side_bar.result_description
            new_desc = descriptions[result_position]

            self._side_bar.result_description = old_desc + new_desc
        
        if result_position != 0 and result_position-1 < len(result_path):
            fr, to, promotion_type = result_path[result_position-1]

            self._chess_game.make_move(fr, to, promotion_type)

        if result_position < len(highlights):
            self._board_highlighter.highlight(highlights[result_position])
    
        self._current_result_position += 1

    # -- UI elements -- #

    def _create_side_panel(self, window_rect: pygame.Rect) -> PuzzleSideBar:
        """"""
        panel = PuzzleSideBar(window_rect, self._info, self._chess_game.current_player, self)

        return panel
    
    @override
    def on_click_after_game_end(self) -> None:
        self.next_result_position()
        

# ---- END DEFINE ---- #

