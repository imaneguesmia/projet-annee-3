from .side_bar_base import SideBarBase, PANEL_WIDTH, PANEL_PADDING

from .puzzles import PuzzleInfo

import image_loader as img

from ..scene_changer_interface import SceneChangeInterface

from ui import NinepatchPanel, Text, MultilineText, TextAlign
from ui.font import FONT_SIZE_TINY, FONT_SIZE_SMALL, FONT_SIZE_MEDIUM
from ui.colors import *

import cpp_chess as cm

import pygame

EVAL_PANEL_TEXT_COLOR = "white"

TITLE_FONT_SIZE = FONT_SIZE_MEDIUM

DESCRIPTION_ORIGIN_X = PANEL_PADDING
DESCRIPTION_ORIGIN_Y = PANEL_PADDING + TITLE_FONT_SIZE + 10

CHILD_WIDTH = PANEL_WIDTH - PANEL_PADDING*2

DESCRIPTION_HEIGHT = 170
DESCRIPTION_FONT_SIZE = FONT_SIZE_SMALL

RESULT_PANEL_HEIGHT = 500
RESULT_PANEL_PADDING = 12

RESULT_PANEL_HEADER_FONT_SIZE = FONT_SIZE_MEDIUM
RESULT_PANEL_BODY_FONT_SIZE = FONT_SIZE_TINY

class PuzzleSideBar(SideBarBase):
    def _create_title(self) -> Text:
        """"""
        return Text(
            f"{"White" if self._player_to_move == cm.Player.White else "Blue"} to move!",
            (self.area.centerx, PANEL_PADDING + TITLE_FONT_SIZE//2),
            fontsize=TITLE_FONT_SIZE
        )
    
    def _create_description(self) -> MultilineText:
        """"""
        text_rect = pygame.Rect(
            DESCRIPTION_ORIGIN_X, DESCRIPTION_ORIGIN_Y,
            CHILD_WIDTH, DESCRIPTION_HEIGHT
        )
        
        return MultilineText(
            text_rect, self._puzzle_description,
            fontsize=DESCRIPTION_FONT_SIZE,
            align=TextAlign.CENTER
        )
    
    def _create_result_panel(self) -> tuple[
        NinepatchPanel, 
        Text, 
        Text,
        MultilineText
    ]:
        """Creates the panel displaying
        """
        rect = pygame.Rect(
            PANEL_PADDING, self._body.dest_rect.bottom,
            CHILD_WIDTH, RESULT_PANEL_HEIGHT
        )

        panel = NinepatchPanel(rect, img.IMAGES.panel(img.PanelTheme.INSET), 31)

        wrong_text = Text(
            "Try again!", panel.area.center,
            fontsize=RESULT_PANEL_HEADER_FONT_SIZE,
            color=(255, 10, 10)
        )
        wrong_text.is_visible = False

        panel.add_child(wrong_text)

        header = Text(
            "Correct!",
            (panel.area.centerx, RESULT_PANEL_PADDING + RESULT_PANEL_HEADER_FONT_SIZE//2),
            fontsize=RESULT_PANEL_HEADER_FONT_SIZE,
            color=(0, 255, 0)
        )
        header.is_visible = False

        result_desc_rect = pygame.Rect(
            RESULT_PANEL_PADDING,
            header.dest_rect.bottom + RESULT_PANEL_BODY_FONT_SIZE,
            CHILD_WIDTH - RESULT_PANEL_PADDING*2,
            RESULT_PANEL_HEIGHT - header.dest_rect.bottom - RESULT_PANEL_PADDING
        )
        result_description = MultilineText(
            result_desc_rect, "",
            align=TextAlign.CENTER,
            fontsize=RESULT_PANEL_BODY_FONT_SIZE,
            color=(255, 255, 255)
        )
        result_description.is_visible = False

        panel.add_child(header)
        panel.add_child(result_description)
        
        return panel, wrong_text, header, result_description

    def __init__(self, 
        window_rect: pygame.Rect,
        puzzle_info: PuzzleInfo,
        player_to_move: cm.Player,
        scene_changer: SceneChangeInterface
    ):
        super().__init__(window_rect, scene_changer)

        self._player_to_move = player_to_move
        self._puzzle_description = puzzle_info.description

        self._title = self._create_title()
        self.add_child(self._title)

        self._body = self._create_description()
        self.add_child(self._body)

        self._result_panel,                             \
        self._wrong_text,                               \
        self._correct_header,                           \
        self._result_description                        \
            = self._create_result_panel()
        
        self.add_child(self._result_panel)

        self.add_child(self._create_quit_button())
    
    def indicate_wrong(self) -> None:
        """Indicates that the player chose the wrong move."""
        self._wrong_text.is_visible = True
    
    def clear_wrong(self) -> None:
        """Hide the wrong move indicator."""
        self._wrong_text.is_visible = False
    
    def indicate_correct(self) -> None:
        """Indicates that the player chose one of the correct moves."""
        self.clear_wrong()

        self._correct_header.is_visible = True
        self._result_description.is_visible = True
    
    @property
    def result_description(self) -> str:
        return self._result_description.text
    @result_description.setter
    def result_description(self, result_description: str) -> None:
        self._result_description.text = result_description