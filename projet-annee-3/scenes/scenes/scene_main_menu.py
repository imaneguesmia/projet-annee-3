from ..scene import Scene
from ..scene_change import SceneId

from ui import Text, Button

import pygame

AQUA_BLUE = (173, 216, 230)  
SAND_COLOR = (237, 201, 175)  
SAND_HOVER = (255, 228, 196) 
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

class scene_MainMenu(Scene):
    def _create_play_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the play button."""
        button_rect = pygame.Rect(window_rect.centerx - 100, window_rect.centery - 40, 200, 80)

        play_button = Button(button_rect, "Play")
        play_button.color = SAND_COLOR
        play_button.hover_color = SAND_HOVER
        play_button.text_color = BLACK
        play_button.text_font = pygame.font.Font(None, 60)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.TEST, {})
            return True

        play_button.on_click = oc

        return play_button
    
    def _create_quit_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the quit button."""
        button_rect = pygame.Rect(window_rect.centerx - 100, window_rect.centery + 60, 200, 80)

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
        super().__init__(window_rect)

        self.bg_color = AQUA_BLUE

        title = Text("Aquachess", (window_rect.centerx, 150), color=BLACK)
        title.font = pygame.font.Font(None, 80)

        self.elements.extend([
            self._create_play_button(window_rect),
            self._create_quit_button(window_rect),
            title
        ])