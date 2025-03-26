import image_loader as img

from ..data_transfer.player_type import PlayerType

from ..scene_changer_interface import SceneChangeInterface, SceneId

from ui import Button, NinepatchPanel
from ui.font import FONT_PATH, FONT_SIZE_MEDIUM
from ui.colors import *

import pygame

from abc import ABC

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

PANEL_WIDTH = 400
PANEL_PADDING = 20

class SideBarBase(NinepatchPanel, ABC):
    def _create_button(self, button_rect: pygame.Rect, text: str) -> Button:
        """Creates a button to be displayed on this panel."""
        button = Button(button_rect, text)
        button.color = SAND_COLOR
        button.hover_color = SAND_HOVER
        button.text_color = BLACK
        button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        return button
    
    def _create_quit_button(self) -> Button:
        """Creates the 'Quit' button."""
        button_rect = pygame.Rect(
            self.area.centerx - BUTTON_WIDTH//2,
            self.area.height - BUTTON_MARGIN - BUTTON_HEIGHT,
            BUTTON_WIDTH, BUTTON_HEIGHT
        )

        quit_button = Button(button_rect, "Quit")
        quit_button.color = SAND_COLOR
        quit_button.hover_color = SAND_HOVER
        quit_button.text_color = BLACK
        quit_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        def quit(_) -> bool:
            self._scene_changer.request_scene_change(SceneId.DIFFICULTY, {})
            return True

        quit_button.on_click = quit

        return quit_button
    
    def _player_type_to_string(self, type: PlayerType) -> str:
        """Returns the string representation of the given player type."""
        match type:
            case PlayerType.HUMAN:
                return "Human"
            case PlayerType.MINIMAX:
                return "Minimax"
            case PlayerType.NEURAL_NET:
                return "Neural Net"

    def __init__(self, 
        window_rect: pygame.Rect,
        scene_changer: SceneChangeInterface
    ):
        panel_rect = pygame.Rect(
            window_rect.width - PANEL_WIDTH, 0,
            PANEL_WIDTH,
            window_rect.height
        )

        super().__init__(panel_rect, img.IMAGES.panel(img.PanelTheme.LEFT), 31)
        
        self._scene_changer = scene_changer

        self.add_child(self._create_quit_button())
        