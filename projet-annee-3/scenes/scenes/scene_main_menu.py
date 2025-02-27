from ..scene import Scene
from ..scene_change import SceneId

from ui import Text, Button
from ui.colors import *

import pygame

MENU_BUTTON_WIDTH = 200
MENU_BUTTON_HEIGHT = 80
bg_image_path = "./projet-annee-3/images/bg.png"

class scene_MainMenu(Scene):
    def _create_play_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the play button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH/2, 
            window_rect.centery - MENU_BUTTON_HEIGHT/2, 
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        play_button = Button(button_rect, "Play")
        play_button.color = SAND_COLOR
        play_button.hover_color = SAND_HOVER
        play_button.text_color = BLACK
        play_button.text_font = pygame.font.Font(None, 60)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.DIFFICULTY, {})
            return True

        play_button.on_click = oc

        return play_button
    
    def _create_quit_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the quit button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH/2, 
            window_rect.centery - MENU_BUTTON_HEIGHT/2 + 100, 
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        quit_button = Button(button_rect, "Quit")
        quit_button.color = SAND_COLOR
        quit_button.hover_color = SAND_HOVER
        quit_button.text_color = BLACK
        quit_button.text_font = pygame.font.Font(None, 60)

        def oc(point: tuple[int, int]) -> bool:
            self.request_quit()
            return True

        quit_button.on_click = oc

        return quit_button

    def __init__(self, window_rect: pygame.Rect):
        super().__init__(window_rect, bg_image_path)

        self.bg_color = AQUA_BLUE

        title = Text("AQUACHESS", (window_rect.centerx, 150), color=BLACK)
        title.font = pygame.font.Font(None, 80)

        self.elements.extend([
            self._create_play_button(window_rect),
            self._create_quit_button(window_rect),
            title
        ])