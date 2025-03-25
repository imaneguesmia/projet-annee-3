from .abstract_chess_scene import AbstractChessScene

from ..data_transfer import PlayerType, PlayerInfo

from .chess_board import ChessBoard
from .chess_board_callback_interface import ChessBoardCallbackInterface
from .promotion_callback_interface import PromotionCallbackInterface

from .promotion_panel import PromotionPanel
from .game_end_panel import GameEndPanel
from .side_bar import SideBar, PANEL_WIDTH as SIDEBAR_WIDTH

import cpp_chess as cm

from ui import Button
from ui.font import FONT_PATH, FONT_SIZE_MEDIUM
from ui.colors import *

from ..scene_change import SceneId

import pygame

from typing import override

# ---- UI constants ---- #

BOARD_SIZE = 800

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

# ---- DEFINE class scene_ChessGame ---- #

class scene_ChessGame(AbstractChessScene, PromotionCallbackInterface):
    def _initialize_engines(self) -> list[cm.AIMoveProvider | None]:
        """Initializes the AI engines."""
        engines: list[cm.AIMoveProvider | None] = []

        for info in self._player_info:
            if info.type == PlayerType.HUMAN:
                engines.append(None)
            else:
                engines.append(self._chess_game.create_ai_player(info.settings))
        
        return engines

    def __init__(self, 
        window_rect: pygame.Rect, 
        white_player: PlayerInfo,
        black_player: PlayerInfo,
        initial_state: str | None = None,
    ):
        self._player_info = (white_player, black_player)

        super().__init__(window_rect, initial_state)

        self._promotion: cm.Move | None = None

        self._promotion_panel = PromotionPanel(self._board.absolute_rect, self)
        self._promotion_panel.is_visible = False
        self._promotion_panel.can_accept_events = False

        self._game_end_panel = self._create_game_end_panel()

        self.elements.append(self._promotion_panel)
        self.elements.append(self._game_end_panel)

        self._side_bar = self._create_side_panel(window_rect)
        self.elements.append(self._side_bar)
        
        self._frames_before_next_turn = 0  # Set to `0` to advance game turn next frame.

        # Initialize AIs and start game

        self._engines = self._initialize_engines()
        self.update_stored_game_data()
    
    @property
    def selected_square(self) -> cm.Position | None: 
        return self._selected_square
    
    @property
    def selected_moves(self) -> list[cm.Move]:
        return self._selected_moves
    
    # -- Game loop -- #

    @override
    def update(self) -> None:
        """"""
        super().update()

        # Handle countdown to next turn.
        if self._frames_before_next_turn > 0:
            self._frames_before_next_turn -= 1
        elif self._frames_before_next_turn == 0:
            self._frames_before_next_turn = -1
            self.game_turn()
    
    def advance_turn(self) -> None:
        """Waits two frames, then advances to the next turn.
        
        This leaves time for the board drawing to catch up.
        """
        self._frames_before_next_turn = 2

    def get_player_type(self, player: cm.Player) -> PlayerType:
        """Gets the type of the given player (human, minimax AI or neural network AI)"""
        return (
            self._player_info[0].type 
            if player == cm.Player.White 
            else self._player_info[1].type
        )
    
    def get_player_engine(self, player: cm.Player) -> cm.AIMoveProvider | None:
        """Gets the engine of the given player (for human players, returns None)"""
        return self._engines[0] if player == cm.Player.White else self._engines[1]
    
    def player_move(self, player: cm.Player) -> None:
        """Does a move depending on the player type.

        Args:
            player_type (PlayerType): The type of player to move.
        """
        engine = self.get_player_engine(player)

        if engine is None:
            # Allow the player to make a move
            self._board.can_accept_events = True
        else:
            # Prevent human from moving
            self._board.can_accept_events = False

            move_to_make = engine.get_move(
                self._extended_game_data
            )

            self.make_move(move_to_make)

    def end_game(self, game_state: cm.GameState) -> None:
        """Display for the end of the game."""
        self._board.can_accept_events = False
        self._board.is_game_over = True

        self._game_end_panel.display_end_state(game_state, self._chess_game.current_player)

    def display_evaluations(self) -> None:
        """Updates the displayed position evaluations."""
        for player in [cm.Player.White, cm.Player.Black]:
            engine = self.get_player_engine(player)

            evaluation = None if engine is None else engine.get_position_value(self._extended_game_data, player)

            self._side_bar.set_evaluation(evaluation, player)
    
    @override
    def update_stored_game_data(self) -> None:
        """Updates the cached game data."""
        super().update_stored_game_data()

        self.display_evaluations()

    def game_turn(self) -> None:
        """Does a game turn, checking if the game is finished then letting the current
        player make a move if it isn't.
        """
        self.update_stored_game_data()

        current_player = self._chess_game.current_player
        game_state = self._game_data.game_state

        if game_state == cm.GameState.INGAME:
            self.player_move(current_player)
        else:
            self.end_game(game_state)

    # -- Game model methods -- #

    def make_move(self, move: cm.Move) -> None:
        """"""
        success = self._chess_game.make_move(move)

        if success:
            # Update data after move
            self._selected_square = None
            self._selected_moves = []

            # Next turn
            self.advance_turn()
            self.update_stored_game_data()
        else:
            print("INVALID MOVE")
            exit()
    
    @override
    def piece_at(self, square: cm.Position) -> cm.Piece:
        return self._game_data.get_piece_at(square)

    @override
    def on_move_chosen(self, move):
        if move.promotion != cm.PType.NoneType:
            self.prompt_promotion(move)
        else:
            self.make_move(move)
    
    def get_legal_moves_from_square(self, square: cm.Position) -> list[cm.Move]:
        filtered_moves = []

        for move in self._legal_moves:
            move_position = cm.Position(move.source)

            if move_position == square:
                filtered_moves.append(move)

        return filtered_moves

    @override
    def prompt_promotion(self, move: cm.Move) -> None:
        self._promotion = move

        self._promotion_panel.set_player(move.player)

        self._promotion_panel.is_visible = True
        self._promotion_panel.can_accept_events = True

        self._board.can_accept_events = False
    
    @override
    def do_current_promotion(self, to_piece: cm.PType) -> None:
        if self._promotion is not None:
            self._promotion_panel.is_visible = False
            self._promotion_panel.can_accept_events = False

            self._board.can_accept_events = True

            self._promotion.promotion = to_piece
            self.make_move(self._promotion)
    
    # -- UI elements -- #

    def _create_side_panel(self, window_rect: pygame.Rect) -> SideBar:
        """"""
        panel = SideBar(window_rect, [info.type for info in self._player_info], self)

        return panel
    
    def _create_game_end_panel(self) -> GameEndPanel:
        """Created the game end panel."""
        panel = GameEndPanel(self._board.absolute_rect)

        # Re-enter the scene with the same parameters
        def retry(_) -> bool:
            self.request_scene_change(SceneId.GAME, {
                "initial_state": self._initial_state,
                "white_player": self._player_info[0],
                "black_player": self._player_info[1]
            })

        def view_board(_) -> bool:
            self.hide_game_end_panel()
            return True

        panel.set_on_retry_clicked(retry)
        panel.set_on_view_board_clicked(view_board)

        return panel
    
    def hide_game_end_panel(self) -> None:
        """Hides the game end panel for board viewing."""
        self._game_end_panel.is_visible = False
        self._game_end_panel.can_accept_events = False

        self._board.can_accept_events = True
    
    @override
    def on_click_after_game_end(self) -> None:
        """Reshows the game end panel after viewing board at the end of the game."""
        self._game_end_panel.is_visible = True
        self._game_end_panel.can_accept_events = True

        self._board.can_accept_events = False

# ---- END DEFINE ---- #

