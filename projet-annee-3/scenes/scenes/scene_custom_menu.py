from ..scene import Scene
from ..scene_change import SceneId
from ui import Text, Button
from ui.font import FONT_PATH, FONT_SIZE_MEDIUM
from ui.slider import Slider
from ui.colors import *
import pygame

MENU_BUTTON_WIDTH = 200
MENU_BUTTON_HEIGHT = 80
BUTTON_SPACING = 50  

bg_image_path = "./resources/images/background.jpeg"

class scene_CustomMenu(Scene):
    def __init__(self, window_rect: pygame.Rect):
        super().__init__(window_rect, bg_image_path)

        self.bg_color = AQUA_BLUE
        self.selected_algorithm = "Minimax"  
        self.ai_depth = 3  

        title = Text("Select AI Settings", (window_rect.centerx, 150), color=BLACK)
        title.font = pygame.font.Font(FONT_PATH, 70)

        minimax_x = window_rect.centerx - MENU_BUTTON_WIDTH - BUTTON_SPACING // 2
        alphabeta_x = window_rect.centerx + BUTTON_SPACING // 2

        self.minimax_button = self._create_algorithm_button(window_rect, "Minimax", minimax_x, "Minimax")
        self.alphabeta_button = self._create_algorithm_button(window_rect, "Alpha-Beta", alphabeta_x, "Alpha-Beta")

               
        self.depth_slider = Slider(
            pygame.Rect(window_rect.centerx - 150, window_rect.centery, 300, 20), 
            min_value=0, max_value=10, initial_value=self.ai_depth
        )


        self.depth_label = Text(f"Depth: {self.ai_depth}", (window_rect.centerx, window_rect.centery - 40), color=BLACK)
        self.depth_label.font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        self.elements.extend([
            title,
            self.minimax_button,
            self.alphabeta_button,
            self._create_start_game_button(window_rect),
            self._create_back_button(window_rect)
        ])

    def _create_algorithm_button(self, window_rect: pygame.Rect, label: str, x_position: int, algorithm: str) -> Button:
        """Creates a horizontally aligned button to select the algorithm."""
        button_rect = pygame.Rect(
            x_position,  
            window_rect.centery - MENU_BUTTON_HEIGHT - 50,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        button = Button(button_rect, label)
        self._update_button_style(button, algorithm)

        def oc(point: tuple[int, int]) -> bool:
            self.selected_algorithm = algorithm
            print(f"Selected Algorithm: {self.selected_algorithm}")  
            self._update_button_style(self.minimax_button, "Minimax")
            self._update_button_style(self.alphabeta_button, "Alpha-Beta")
            return True

        button.on_click = oc
        return button

    def _update_button_style(self, button: Button, algorithm: str):
        """Updates button style to indicate selection."""
        if self.selected_algorithm == algorithm:
            button.color = (255, 200, 100) 
        else:
            button.color = SAND_COLOR

        button.hover_color = SAND_HOVER
        button.text_color = BLACK
        button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

    def _create_start_game_button(self, window_rect: pygame.Rect) -> Button:
        """Creates the button to start the game with the chosen algorithm."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2,
            window_rect.centery + 50,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        start_button = Button(button_rect, "Play")
        start_button.color = SAND_COLOR
        start_button.hover_color = SAND_HOVER
        start_button.text_color = BLACK
        start_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        def oc(point: tuple[int, int]) -> bool:
            self.ai_depth = self.depth_slider.get_value()  
            print(f"Starting the game with {self.selected_algorithm} and a depth of {self.ai_depth}") 
            self.request_scene_change(SceneId.GAME, {})
            return True

        start_button.on_click = oc
        return start_button

    def _create_back_button(self, window_rect: pygame.Rect) -> Button:
        """Creates the button to return to the previous menu."""
        button_rect = pygame.Rect(
            window_rect.centerx - MENU_BUTTON_WIDTH / 2,
            window_rect.centery + 150,
            MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
        )

        back_button = Button(button_rect, "Back")
        back_button.color = SAND_COLOR
        back_button.hover_color = SAND_HOVER
        back_button.text_color = BLACK
        back_button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        def oc(point: tuple[int, int]) -> bool:
            print("Returning to the previous menu")
            self.request_scene_change(SceneId.DIFFICULTY, {})
            return True

        back_button.on_click = oc
        return back_button
    
    def handle_event(self, event):
        """Handles slider events."""
        super().handle_event(event)
        self.depth_slider.handle_event(event)
        self.depth_label.text = f"Depth: {self.depth_slider.get_value()}"

    def draw(self, surface):
        """Draws the scene and the slider."""
        super().draw(surface)
        self.depth_slider.draw(surface)
