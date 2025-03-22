from ..scene import Scene
from ..scene_change import SceneId
from ..data_transfer import PlayerType, PlayerInfo

from .settings_panel import AIOptionsPanel

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

class scene_AISettings(Scene):
    def _create_bottom_buttons(self, window_rect: pygame.Rect) -> list[Button]:
        """Creates and returns the bottom buttons (back and play)."""
        buttons = []

        for button_type in ["Back", "Play"]:
            button_rect = pygame.Rect(
                0, 0,
                MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
            )

            if button_type == "Back":
                button_rect.bottomright = (
                    window_rect.centerx - BUTTON_SPACING_X//2, 
                    window_rect.bottom - Y_PADDING
                )
            else:
                button_rect.bottomleft = (
                    window_rect.centerx + BUTTON_SPACING_X//2, 
                    window_rect.bottom - Y_PADDING
                )

            button = Button(button_rect, button_type)
            button.color = SAND_COLOR
            button.hover_color = SAND_HOVER
            button.text_color = BLACK
            button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

            buttons.append(button)

        return buttons
    
    def _create_title(self) -> Text:
        """"""
        title_center = (self.window_rect.centerx, Y_PADDING + FONT_SIZE_XL/2)

        title = Text("SETTINGS", title_center, align=TextAlign.CENTER)
        title.font = pygame.font.Font(TITLE_FONT_PATH, FONT_SIZE_XL)

        return title
    
    def _on_back(self, _) -> bool:
        self.request_scene_change(SceneId.DIFFICULTY, {})

        return True
    
    def _on_play(self, _) -> bool:
        info: list[PlayerInfo] = []

        for type, settings in zip(self._player_types, self._settings_panels):
            info.append(PlayerInfo(
                type,
                settings.get_settings()
            ))
        
        self.request_scene_change(SceneId.GAME, {
            "white_player": info[0],
            "black_player": info[1]
        })

        return True

    def __init__(self, window_rect: pygame.Rect, player_types: list[PlayerType]):
        super().__init__(window_rect, bg_image_path)

        self.bg_color = AQUA_BLUE

        self._player_types = player_types
        self._settings_panels: list[AIOptionsPanel] = []

        for i, type in enumerate(player_types):
            self._settings_panels.append(AIOptionsPanel(window_rect, type, i == 1))
        
        back, play = self._create_bottom_buttons(window_rect)

        back.on_click = self._on_back
        play.on_click = self._on_play

        self.elements.extend(self._settings_panels)
        self.elements.extend([
            self._create_title(), back, play
        ])
