from ..scene import Scene
from ..scene_change import SceneId

from .main_menu_panels import PuzzleChoicePanel, NormalGamePanel

from ui import Button, Text, TextAlign
from ui.font import FONT_PATH, TITLE_FONT_PATH, FONT_SIZE_MEDIUM, FONT_SIZE_XL
from ui.colors import *

import pygame

MENU_BUTTON_WIDTH = 200
MENU_BUTTON_HEIGHT = 80
BUTTON_SPACING_X = 150  # horizontal spacing between buttons
BUTTON_SPACING_Y = 100  # vertical spacing between rows

Y_PADDING = 60

bg_image_path = "./resources/images/testbg.png"

class scene_DifficultyMenu(Scene):
    def _create_back_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the back button."""
        button_rect = pygame.Rect(
            0, 0,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )
        button_rect.midbottom = (window_rect.centerx, window_rect.bottom - Y_PADDING)

        back_button = Button(button_rect, "Back")
        back_button.color = SAND_COLOR
        back_button.hover_color = SAND_HOVER
        back_button.text_color = BLACK
        back_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.MAINMENU, {})
            return True

        back_button.on_click = oc
        return back_button
    
    def _create_title(self) -> Text:
        """"""
        title_center = (self.window_rect.centerx, Y_PADDING + FONT_SIZE_XL/2)

        title = Text("SELECT MODE", title_center, align=TextAlign.CENTER)
        title.font = pygame.font.Font(TITLE_FONT_PATH, FONT_SIZE_XL)

        return title

    def __init__(self, window_rect: pygame.Rect):
        super().__init__(window_rect, bg_image_path)

        self.bg_color = AQUA_BLUE

        self._puzzle_choice_panel = PuzzleChoicePanel(window_rect, self)
        self._normal_game_panel = NormalGamePanel(window_rect, self)

        self.elements.extend([
            self._puzzle_choice_panel, self._normal_game_panel,
            self._create_title(),
            self._create_back_button(window_rect)
        ])
