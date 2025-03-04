from ..scene import Scene
from ..scene_change import SceneId

from ui import Text, Button
from ui.colors import *

import pygame

MENU_BUTTON_WIDTH = 200
MENU_BUTTON_HEIGHT = 80


bg_image_path = "./projet-annee-3/images/background.jpeg"

class scene_DifficultyMenu(Scene):
    def _create_difficulty_button(self, window_rect: pygame.Rect, label: str, y_offset: int, scene_id: SceneId) -> Button:
        """Creates and returns a difficulty button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2,
            window_rect.centery - MENU_BUTTON_HEIGHT / 2 + y_offset,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        difficulty_button = Button(button_rect, label)
        difficulty_button.color = SAND_COLOR
        difficulty_button.hover_color = SAND_HOVER
        difficulty_button.text_color = BLACK
        difficulty_button.text_font = pygame.font.Font(None, 50)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.GAME, {})
            return True

        difficulty_button.on_click = oc
        return difficulty_button
    
    def _create_custom_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the custom AI settings button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2,
            window_rect.centery - MENU_BUTTON_HEIGHT / 2 + 200,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        custom_button = Button(button_rect, "Custom")
        custom_button.color = SAND_COLOR
        custom_button.hover_color = SAND_HOVER
        custom_button.text_color = BLACK
        custom_button.text_font = pygame.font.Font(None, 50)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.CUSTOM, {}) 
            return True

        custom_button.on_click = oc
        return custom_button


    def _create_back_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the back button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2,
            window_rect.centery - MENU_BUTTON_HEIGHT / 2 + 300,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        back_button = Button(button_rect, "Back")
        back_button.color = SAND_COLOR
        back_button.hover_color = SAND_HOVER
        back_button.text_color = BLACK
        back_button.text_font = pygame.font.Font(None, 50)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.MAINMENU, {})
            return True

        back_button.on_click = oc
        return back_button

    def __init__(self, window_rect: pygame.Rect):
        super().__init__(window_rect, bg_image_path)

        self.bg_color = AQUA_BLUE

        title = Text("Select Difficulty", (window_rect.centerx, 150), color=BLACK)
        title.font = pygame.font.Font(None, 70)

        self.elements.extend([
            title,
            self._create_difficulty_button(window_rect, "Easy", -100, SceneId.GAME),
            self._create_difficulty_button(window_rect, "Medium", 0, SceneId.GAME),
            self._create_difficulty_button(window_rect, "Hard", 100, SceneId.GAME),
            self._create_custom_button(window_rect),
            self._create_back_button(window_rect)
        ])
