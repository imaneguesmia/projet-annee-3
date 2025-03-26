from .title_panel import TitlePanel, HEADER_FONT_SIZE, PANEL_PADDING_X, PANEL_PADDING_Y

from .puzzles import PuzzleInfo, PUZZLES

from ..scene_change import SceneId
from ..scene_changer_interface import SceneChangeInterface

from ..data_transfer import PlayerType

from ui import Text, TextAlign, Button, CyclingOptions
from ui.font import FONT_PATH, FONT_SIZE_TINY, FONT_SIZE_SMALL, FONT_SIZE_MEDIUM
from ui.colors import *

import pygame

PANEL_WIDTH = 600
PANEL_HEIGHT = 500

PANEL_SPACING = 40

PANEL_PADDING_X = 20
PANEL_PADDING_Y = 5

PUZZLE_BUTTON_SPACING = 10
PUZZLE_BUTTON_HEIGHT = (PANEL_HEIGHT - (PANEL_PADDING_Y*2 + HEADER_FONT_SIZE)) // len(PUZZLES) - PUZZLE_BUTTON_SPACING
PUZZLE_DESCRIPTION_X = 250

class PuzzleChoicePanel(TitlePanel):
    def _create_puzzle_button(self, index: int, puzzle: PuzzleInfo) -> Button:
        """Creates a button to select a puzzle.

        Args:
            description (str): The description of the puzzle.
            index (int): The index of the button. Determines its y-position.

        Returns:
            Button: The button object.
        """
        button_rect = pygame.Rect(0, 0, 0, 0)

        button_rect.width = PANEL_WIDTH - PANEL_PADDING_X*2
        button_rect.height = PUZZLE_BUTTON_HEIGHT

        button_top = PANEL_PADDING_Y + HEADER_FONT_SIZE
        button_top += (PUZZLE_BUTTON_HEIGHT + PUZZLE_BUTTON_SPACING) * index

        button_rect.centerx = self.area.centerx
        button_rect.top = button_top

        button = Button(button_rect, f"Puzzle {index+1}")

        button.color = SAND_COLOR
        button.hover_color = SAND_HOVER

        button.text_align = TextAlign.LEFT
        button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        button_desc = Text(
            puzzle.subtitle, (PUZZLE_DESCRIPTION_X, button.area.centery), 
            align=TextAlign.LEFT, fontsize=FONT_SIZE_TINY
        )
        button.add_child(button_desc)

        def on_puzzle_click(_) -> bool:
            self.scene_changer.request_scene_change(SceneId.PUZZLE, {
                "puzzle_info": puzzle
            })

            return True
        
        button.on_click = on_puzzle_click

        return button

    def __init__(self, window_rect: pygame.Rect, scene_changer: SceneChangeInterface):
        panel_rect = pygame.Rect(0, 0, 0, 0)

        panel_rect.width = PANEL_WIDTH
        panel_rect.height = PANEL_HEIGHT

        panel_rect.left = window_rect.centerx - PANEL_SPACING//2 - PANEL_WIDTH
        panel_rect.centery = window_rect.centery

        super().__init__(panel_rect, "Puzzles")

        self.scene_changer = scene_changer
        
        for i, puzzle in enumerate(PUZZLES):
            puzzle_button = self._create_puzzle_button(i, puzzle)

            self.add_child(puzzle_button)

OPTIONS_CENTER_WIDTH = 200
OPTIONS_CENTER_HEIGHT = 50
OPTIONS_CENTER_FONTSIZE = FONT_SIZE_SMALL

OPTIONS_ARROW_SIZE = 40
OPTIONS_ARROW_SPACING = 15

OPTIONS_Y_OFFSET = 125 + PANEL_PADDING_Y
OPTIONS_Y_SPACING = 90

OPTIONS_LABEL_Y_SPACING = 30
OPTIONS_LABEL_FONTSIZE = FONT_SIZE_MEDIUM

PLAY_BUTTON_WIDTH = 200
PLAY_BUTTON_HEIGHT = 80
PLAY_BUTTON_Y_MARGIN = 10

class NormalGamePanel(TitlePanel):
    def _create_player_options(self, y_offset: int, label_text: str) -> tuple[Text, CyclingOptions]:
        """Creates the UI elements to choose the type of each player."""
        label = Text(label_text, (self.area.centerx, y_offset), fontsize=OPTIONS_LABEL_FONTSIZE)

        center_rect = pygame.Rect(0, 0, OPTIONS_CENTER_WIDTH, OPTIONS_CENTER_HEIGHT)
        center_rect.center = (
            self.area.centerx,
            label.dest_rect.bottom + OPTIONS_LABEL_Y_SPACING
        )

        options = ["Human", "Minimax", "Neural Net"]

        cycler = CyclingOptions(center_rect, OPTIONS_ARROW_SIZE, OPTIONS_ARROW_SPACING, options)
        cycler.center_text_font = pygame.font.Font(FONT_PATH, OPTIONS_CENTER_FONTSIZE)
        cycler.center_color = SAND_HOVER
        cycler.arrow_color = (0, 0, 0)
        cycler.arrow_hover_color = (100, 100, 100)

        return label, cycler
    
    def _play_with_options(self, _) -> None:
        """Starts a game with the selected player types."""
        options = [PlayerType.HUMAN, PlayerType.MINIMAX, PlayerType.NEURAL_NET]

        self._scene_changer.request_scene_change(SceneId.SETTINGS, {
            "player_types": [options[cycler.get_selected_option()] for cycler in self._cyclers]
        })
    
    def _create_play_button(self) -> Button:
        """Creates the button to play the game with the selected player types."""
        rect = pygame.Rect(0, 0, PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT)
        rect.midbottom = (
            self.area.centerx,
            self.area.bottom - PANEL_PADDING_Y - PLAY_BUTTON_Y_MARGIN
        )

        button = Button(rect, "Play")

        button.color = SAND_COLOR
        button.hover_color = SAND_HOVER

        button.on_click = self._play_with_options 

        return button

    def __init__(self, window_rect: pygame.Rect, scene_changer: SceneChangeInterface):
        panel_rect = pygame.Rect(0, 0, 0, 0)

        panel_rect.width = PANEL_WIDTH
        panel_rect.height = PANEL_HEIGHT

        panel_rect.right = window_rect.centerx + PANEL_SPACING//2 + PANEL_WIDTH
        panel_rect.centery = window_rect.centery

        super().__init__(panel_rect, "Play")

        self._scene_changer = scene_changer

        self._cyclers: list[CyclingOptions] = []

        for i in range(2):
            index_offset = i * (OPTIONS_CENTER_HEIGHT + OPTIONS_Y_SPACING) + OPTIONS_Y_OFFSET

            text, cycler = self._create_player_options(index_offset, ["White", "Blue"][i] + " Player")

            self.add_child(text)
            self.add_child(cycler)

            self._cyclers.append(cycler)

        print(self._cyclers)
        
        self.add_child(self._create_play_button())
