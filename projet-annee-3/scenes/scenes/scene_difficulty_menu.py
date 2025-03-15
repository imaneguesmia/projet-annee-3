from ..scene import Scene
from ..scene_change import SceneId

from ui import Text, Button
from ui.colors import *

import pygame

MENU_BUTTON_WIDTH = 200
MENU_BUTTON_HEIGHT = 80
BUTTON_SPACING_X = 150  # horizontal spacing between buttons
BUTTON_SPACING_Y = 100  # vertical spacing between rows


bg_image_path = "./projet-annee-3/images/beach.jpg"

class scene_DifficultyMenu(Scene):
    def _create_difficulty_button(self, window_rect: pygame.Rect, label: str, x_offset: int, y_offset: int, scene_id: SceneId) -> Button:
        """Creates and returns a difficulty button."""

        # Utilise l'origine calculée plus haut
        button_rect = pygame.Rect(
            self.group_origin_x + x_offset,
            self.group_origin_y + y_offset,
            MENU_BUTTON_WIDTH,
            MENU_BUTTON_HEIGHT
        )

        difficulty_button = Button(button_rect, label)
        difficulty_button.color = SAND_COLOR
        difficulty_button.hover_color = SAND_HOVER
        difficulty_button.text_color = BLACK
        difficulty_button.text_font = pygame.font.Font(None, 50)

        def oc(point: tuple[int, int]) -> bool:
            self.request_scene_change(scene_id, {})
            return True

        difficulty_button.on_click = oc
        return difficulty_button




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

        # Load the title image
        self.title_image = pygame.image.load("./projet-annee-3/images/select_puzzle.png").convert_alpha()


        self.bg_color = AQUA_BLUE

        # 👇 Nouveau centrage du groupe sur toute la fenêtre
        total_group_width = MENU_BUTTON_WIDTH * 2 + BUTTON_SPACING_X
        total_group_height = MENU_BUTTON_HEIGHT * 2 + BUTTON_SPACING_Y

        self.group_origin_x = (window_rect.width - total_group_width) // 2
        self.group_origin_y = (window_rect.height - total_group_height) // 2

        self.elements.extend([

            # Top-left button (Puzzle 1)
            self._create_difficulty_button(window_rect, "Puzzle 1", 0, 0, SceneId.GAME),

            # Top-right button (Puzzle 2)
            self._create_difficulty_button(window_rect, "Puzzle 2",
                MENU_BUTTON_WIDTH + BUTTON_SPACING_X, 0, SceneId.GAME),

            # Bottom-left button (Puzzle 3)
            self._create_difficulty_button(window_rect, "Puzzle 3",
                0, MENU_BUTTON_HEIGHT + BUTTON_SPACING_Y, SceneId.GAME),

            # Bottom-right button (Puzzle 4)
            self._create_difficulty_button(window_rect, "Puzzle 4",
                MENU_BUTTON_WIDTH + BUTTON_SPACING_X,
                MENU_BUTTON_HEIGHT + BUTTON_SPACING_Y,
                SceneId.GAME
            ),

            self._create_back_button(window_rect)
        ])
        
    def draw(self, surface):
        # Call base draw for background + UI elements
        super().draw(surface)

        # Draw title image at the top center
        title_rect = self.title_image.get_rect(center=(self.window_rect.centerx, 200))
        surface.blit(self.title_image, title_rect)


