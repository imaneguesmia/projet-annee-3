from .title_panel import TitlePanel, HEADER_FONT_SIZE, PANEL_PADDING_X, PANEL_PADDING_Y

from .puzzles import PuzzleInfo, PUZZLES

from ..scene_change import SceneId
from ..scene_changer_interface import SceneChangeInterface

from ..data_transfer import PlayerType

from ui import Text, TextAlign, Button, CyclingOptions, Checkbox
from ui.font import FONT_PATH, FONT_SIZE_TINY, FONT_SIZE_SMALL, FONT_SIZE_MEDIUM
from ui.colors import *

import cpp_chess as cm

import pygame

PANEL_WIDTH = 600
PANEL_HEIGHT = 500

PANEL_SPACING = 40

PANEL_PADDING_X = 20
PANEL_PADDING_Y = 5

OPTION_FONT_SIZE = FONT_SIZE_SMALL
CHECKBOX_BORDER_SIZE = 6

OPTIONS_X_ORIGIN = 100

DEPTH_OPTION_Y_ORIGIN = 115
DEPTH_CYCLER_WIDTH = 50
DEPTH_CYCLER_ARROW_SIZE = OPTION_FONT_SIZE // 2
DEPTH_CYCLER_ARROW_SPACING = 10

CHECKBOX_OPTIONS_Y_ORIGIN = DEPTH_OPTION_Y_ORIGIN + 75

OPTIONS_SPACING = 20

class AIOptionsPanel(TitlePanel):
    def _create_depth_cycler(self) -> CyclingOptions:
        """"""
        title = Text(
            "Depth :",
            (OPTIONS_X_ORIGIN, DEPTH_OPTION_Y_ORIGIN),
            align=TextAlign.LEFT,
            fontsize=FONT_SIZE_MEDIUM
        )

        center_rect = pygame.Rect(0, 0, DEPTH_CYCLER_WIDTH, OPTION_FONT_SIZE)
        center_rect.midleft = (
            title.dest_rect.right + OPTIONS_SPACING + DEPTH_CYCLER_ARROW_SIZE + DEPTH_CYCLER_ARROW_SPACING,
            DEPTH_OPTION_Y_ORIGIN - 3
        )

        cycler = CyclingOptions(
            center_rect, DEPTH_CYCLER_ARROW_SIZE, DEPTH_CYCLER_ARROW_SPACING,
            ["1", "2", "3", "4", "5"],
            initial_index=4
        )
        cycler.center_text_font = pygame.font.Font(FONT_PATH, OPTION_FONT_SIZE)
        cycler.center_color = (0, 0, 0, 0)
        cycler.arrow_color = "black"
        cycler.arrow_hover_color = (100, 100, 100)

        self.add_child(title)
        self.add_child(cycler)

        return cycler

    def _create_minimax_heuristic_options(self) -> list[Checkbox]:
        """"""
        y = CHECKBOX_OPTIONS_Y_ORIGIN

        self.add_child(Text(
            "Heuristics",
            (OPTIONS_X_ORIGIN, y),
            align=TextAlign.LEFT,
            fontsize=FONT_SIZE_MEDIUM
        ))

        minimax_options = ["Material", "Piece Square Table", "Mobility", "Pawn Structure"]
        checkboxes: list[Checkbox] = []

        for option in minimax_options:
            y += OPTION_FONT_SIZE + OPTIONS_SPACING

            checkbox = Checkbox(
                (OPTIONS_X_ORIGIN + OPTION_FONT_SIZE, y), 
                OPTION_FONT_SIZE, 
                CHECKBOX_BORDER_SIZE,
                checked=True
            )
            checkbox.checked_color = (0, 100, 0)

            text = Text(
                option, 
                (checkbox.dest_rect.right + OPTIONS_SPACING, y),
                align=TextAlign.LEFT,
                fontsize=OPTION_FONT_SIZE
            )

            self.add_child(checkbox)
            self.add_child(text)

            checkboxes.append(checkbox)
        
        return checkboxes


    def __init__(self, window_rect: pygame.Rect, type: PlayerType, right_side: bool):
        panel_rect = pygame.Rect(0, 0, 0, 0)

        panel_rect.width = PANEL_WIDTH
        panel_rect.height = PANEL_HEIGHT

        if right_side:
            panel_rect.right = window_rect.centerx + PANEL_SPACING//2 + PANEL_WIDTH
        else:
            panel_rect.left = window_rect.centerx - PANEL_SPACING//2 - PANEL_WIDTH

        panel_rect.centery = window_rect.centery

        super().__init__(panel_rect, "")

        self._player_type = type

        title_text = f"Player {"2" if right_side else "1"}: "

        match type:
            case PlayerType.HUMAN:
                self.set_title(title_text + "Human")
            case PlayerType.MINIMAX:
                self.set_title(title_text + "Minimax")

                self._depth = self._create_depth_cycler()
                self._options = self._create_minimax_heuristic_options()
            case PlayerType.NEURAL_NET:
                self.set_title(title_text + "Neural Net")

                self._depth = self._create_depth_cycler()
    
    def get_settings(self) -> cm.EvaluatorSettings | None:
        """"""
        match self._player_type:
            case PlayerType.HUMAN:
                settings = None
            case PlayerType.MINIMAX:
                settings = cm.EvaluatorSettings()

                settings.type = cm.EvaluatorType.MINIMAX

                settings.depth = self._depth.get_selected_option() + 1

                settings.material           = self._options[0].checked
                settings.piece_square_table = self._options[1].checked
                settings.mobility           = self._options[2].checked
                settings.pawn_structure     = self._options[3].checked
                # settings.king_safety        = self._options[4].checked
                settings.king_safety        = False
            case PlayerType.NEURAL_NET:
                settings = cm.EvaluatorSettings()

                settings.type = cm.EvaluatorType.NEURAL_NET

                settings.depth = self._depth.get_selected_option() + 1

        return settings
