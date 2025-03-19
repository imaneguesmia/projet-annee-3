from ..scene import Scene

from ..data_transfer.player_type import PlayerType

from .chess_board import ChessBoard
from .chess_board_callback_interface import ChessBoardCallbackInterface

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

class scene_ChessGame(Scene, ChessBoardCallbackInterface):

    def __init__(self, 
        window_rect: pygame.Rect, 
        initial_state: str | None = None,
        white_player: PlayerType = PlayerType.HUMAN,
        black_player: PlayerType = PlayerType.HUMAN
    ):
        super().__init__(window_rect)

        self._initial_state = initial_state

        self._player_types = (white_player, black_player)

        self._chess_game = cm.GameManager() if initial_state is None else cm.GameManager(initial_state)
        self._game_data: cm.GameData = None
        self._extended_game_data: cm.ExtendedGameData = None

        self._selected_square: cm.Position | None = None
        self._selected_moves: list[cm.Move] = []

        self._promotion: cm.Move | None = None

        # Centrer l'échiquier de 800x800 dans la fenêtre 1920x1080
        self.board_x = (window_rect.width - SIDEBAR_WIDTH) // 2 - BOARD_SIZE // 2
        self.board_y = (window_rect.height - BOARD_SIZE) // 2

        self._board = ChessBoard(
            pygame.Rect(self.board_x, self.board_y, BOARD_SIZE, BOARD_SIZE),
            self
        )

        self._promotion_panel = PromotionPanel(self._board.absolute_rect, self)
        self._promotion_panel.is_visible = False
        self._promotion_panel.can_accept_events = False

        self._game_end_panel = self._create_game_end_panel()

        self.elements.append(self._board)
        self.elements.append(self._promotion_panel)
        self.elements.append(self._game_end_panel)

        self._side_bar = self._create_side_panel(window_rect)
        self.elements.append(self._side_bar)

        # Initialize AI engines

        if PlayerType.MINIMAX in self._player_types:
            eval_settings = cm.EvaluatorSettings()

            eval_settings.material = True
            eval_settings.piece_square_table = True
            eval_settings.mobility = True
            eval_settings.pawn_structure = True

            self._minimax_engine = self._chess_game.create_minimax_player(5, eval_settings)
        if PlayerType.NEURAL_NET in self._player_types:
            self._neural_net_engine: cm.AIMoveProvider = ...
        
        self._frames_before_next_turn = 0  # Set to `0` to advance game turn next frame.
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
        return self._player_types[0] if player == cm.Player.White else self._player_types[1]
    
    def player_move(self, player_type: PlayerType) -> None:
        """Does a move depending on the player type.

        Args:
            player_type (PlayerType): The type of player to move.
        """
        match player_type:
            case PlayerType.HUMAN:
                # Allow the player to make a move
                self._board.can_accept_events = True
            case PlayerType.MINIMAX:
                # Prevent human from moving
                self._board.can_accept_events = False

                move_to_make = self._minimax_engine.get_move(
                    self._extended_game_data
                )

                self.make_move(move_to_make)
            case PlayerType.NEURAL_NET:
                # Prevent human from moving
                self._board.can_accept_events = False
    
    def end_game(self, game_state: cm.GameState) -> None:
        """Display for the end of the game."""
        self._board.can_accept_events = False
        self._board.is_game_over = True

        self._game_end_panel.display_end_state(game_state, self._chess_game.current_player)

    def display_evaluations(self) -> None:
        """Updates the displayed position evaluations."""

        for player in [cm.Player.White, cm.Player.Black]:
            player_type = self.get_player_type(player)

            match player_type:
                case PlayerType.HUMAN:
                    evaluation = None
                case PlayerType.MINIMAX:
                    evaluation = self._minimax_engine.get_position_value(self._extended_game_data, player)
                case PlayerType.NEURAL_NET:
                    evaluation = self._neural_net_engine.get_position_value(self._extended_game_data, player)

            self._side_bar.set_evaluation(evaluation, player)
    
    def update_stored_game_data(self) -> None:
        """Updates the cached game data."""
        self._game_data = self._chess_game.game_data()
        self._extended_game_data = self._chess_game.extended_game_data()

        self.display_evaluations()

    def game_turn(self) -> None:
        """Does a game turn, checking if the game is finished then letting the current
        player make a move if it isn't.
        """
        self.update_stored_game_data()

        current_player = self._chess_game.current_player
        game_state = self._game_data.game_state

        if game_state == cm.GameState.INGAME:
            current_player_type = self.get_player_type(current_player)

            self.player_move(current_player_type)
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

    def select_square(self, square: cm.Position) -> None:
        """"""
        piece_on_square = self._game_data.get_piece_at(square)
        current_player = self._game_data.current_player

        # Update legal moves
        self._legal_moves = self._game_data.get_current_legals(False)

        # Select the clicked piece if it belongs to the current player
        if not piece_on_square.is_none() and piece_on_square.get_player() == current_player:
            self._selected_square = square
            self._selected_moves = self.get_legal_moves_from_square(square)

            print(f"Selected piece : {piece_on_square.get_type()} ({piece_on_square.get_player()})") #  debug selection

        # Do a move if a piece is already selected and a valid target was clicked
        elif self._selected_square and any(cm.Position(move.target) == square for move in self._selected_moves):
            move_to_do = next((move for move in self._selected_moves if cm.Position(move.target) == square), None)

            if move_to_do:
                if move_to_do.promotion != cm.PType.NoneType:
                    self.prompt_promotion(move_to_do)
                else:
                    self.make_move(move_to_do)

        else:
            self._selected_square = None
            self._selected_moves = []
    
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
        panel = SideBar(window_rect, self._player_types)

        panel.add_child(self._create_quit_button(panel.area))

        return panel

    def _create_quit_button(self, panel_area: pygame.Rect) -> Button:
        """Creates a quit button on the right side of the screen."""
        button_rect = pygame.Rect(
            panel_area.centerx - BUTTON_WIDTH//2,
            panel_area.height - BUTTON_MARGIN - BUTTON_HEIGHT,
            BUTTON_WIDTH, BUTTON_HEIGHT
        )

        quit_button = Button(button_rect, "Quit")
        quit_button.color = SAND_COLOR
        quit_button.hover_color = SAND_HOVER
        quit_button.text_color = BLACK
        quit_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        def quit(_) -> bool:
            self.request_scene_change(SceneId.MAINMENU, {})
            return True

        quit_button.on_click = quit

        return quit_button
    
    def _create_game_end_panel(self) -> GameEndPanel:
        """Created the game end panel."""
        panel = GameEndPanel(self._board.absolute_rect)

        # Re-enter the scene with the same parameters
        def retry(_) -> bool:
            self.request_scene_change(SceneId.GAME, {
                "initial_state": self._initial_state,
                "white_player": self._player_types[0],
                "black_player": self._player_types[1]
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
    def reshow_game_end_panel(self) -> None:
        """Reshows the game end panel after viewing board at the end of the game."""
        self._game_end_panel.is_visible = True
        self._game_end_panel.can_accept_events = True

        self._board.can_accept_events = False

# ---- END DEFINE ---- #

