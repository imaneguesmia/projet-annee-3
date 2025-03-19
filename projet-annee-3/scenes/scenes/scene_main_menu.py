from ..scene import Scene
from ..scene_change import SceneId

from ui import Button
from ui.font import FONT_PATH, FONT_SIZE_LARGE
from ui.colors import *

import pygame

MENU_BUTTON_WIDTH = 200
MENU_BUTTON_HEIGHT = 80
BUTTON_SPACING = 100  # Espace entre les boutons

bg_image_path = "./resources/images/testbg.png"
button_sprite_path = "./resources/images/button_sprite.png"

class scene_MainMenu(Scene):
    def __init__(self, window_rect: pygame.Rect):
        super().__init__(window_rect, bg_image_path)

        self.bg_color = AQUA_BLUE

        # Load and resize the button sprite
        self.button_sprite = pygame.image.load(button_sprite_path)
        self.button_sprite = pygame.transform.scale(self.button_sprite, (MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT))

        # Create buttons
        self.play_button = self._create_play_button(window_rect)
        self.quit_button = self._create_quit_button(window_rect)

        # Title
        self.title_image = pygame.image.load("./resources/images/main_title.png").convert_alpha()


        self.elements.extend([
            self.play_button,
            self.quit_button,
        ])

    def _create_play_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the play button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2, 
            window_rect.centery - MENU_BUTTON_HEIGHT / 2 - BUTTON_SPACING // 2, 
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        play_button = Button(button_rect, "Play")
        play_button.color = SAND_COLOR
        play_button.hover_color = SAND_HOVER
        play_button.text_color = BLACK
        play_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_LARGE)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(SceneId.DIFFICULTY, {})
            return True

        play_button.on_click = oc
        return play_button

    def _create_quit_button(self, window_rect: pygame.Rect) -> Button:
        """Creates and returns the quit button."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2, 
            window_rect.centery - MENU_BUTTON_HEIGHT / 2 + BUTTON_SPACING // 2, 
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        quit_button = Button(button_rect, "Quit")
        quit_button.color = SAND_COLOR
        quit_button.hover_color = SAND_HOVER
        quit_button.text_color = BLACK
        quit_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_LARGE)

        def oc(point: tuple[int, int]) -> bool:
            self.request_quit()
            return True

        quit_button.on_click = oc
        return quit_button

    def draw(self, surface):
        """Draws the menu, including buttons and sprites."""
        super().draw(surface)  # Draw the standard scene elements

        title_rect = self.title_image.get_rect(center=(self.window_rect.centerx, 250))
        surface.blit(self.title_image, title_rect)
        
        # Draw the button sprite on each button
        for button in [self.play_button, self.quit_button]:
            surface.blit(self.button_sprite, button.dest_rect.topleft)
