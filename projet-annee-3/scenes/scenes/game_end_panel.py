import image_loader as img

from ui import Button, NinepatchPanel, Text
from ui.font import FONT_PATH, FONT_SIZE_MEDIUM, FONT_SIZE_LARGE
from ui.colors import *

import cpp_chess as cm

import pygame

PANEL_WIDTH = 600
PANEL_HEIGHT = 200
PANEL_PADDING = 20

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

class GameEndPanel(NinepatchPanel):
    def _create_button(self, button_rect: pygame.Rect, text: str) -> Button:
        """Creates a button to be displayed on this panel."""
        button = Button(button_rect, text)
        button.color = SAND_COLOR
        button.hover_color = SAND_HOVER
        button.text_color = BLACK
        button.text_font = pygame.font.Font(FONT_PATH, FONT_SIZE_MEDIUM)

        return button
    
    def _create_retry_button(self) -> Button:
        """Creates the 'Retry' button."""
        rect = pygame.Rect(
            (self.area.width*3)//4 - BUTTON_WIDTH//2,
            self.area.height - PANEL_PADDING - BUTTON_HEIGHT,
            BUTTON_WIDTH, BUTTON_HEIGHT
        )

        button = self._create_button(rect, "Retry")

        return button

    def _create_view_button(self) -> Button:
        """Creates the 'View Board' button."""
        rect = pygame.Rect(
            (self.area.width)//4 - BUTTON_WIDTH//2,
            self.area.height - PANEL_PADDING - BUTTON_HEIGHT,
            BUTTON_WIDTH, BUTTON_HEIGHT
        )

        button = self._create_button(rect, "View Board")

        return button

    def __init__(self, board_rect: pygame.Rect):
        panel_rect = pygame.Rect(
            board_rect.centerx - PANEL_WIDTH/2 - PANEL_PADDING,
            board_rect.centery - PANEL_HEIGHT/2 - PANEL_PADDING, 
            PANEL_WIDTH + PANEL_PADDING*2,
            PANEL_HEIGHT + PANEL_PADDING*2
        )

        super().__init__(panel_rect, img.IMAGES.panel(img.PanelTheme.TEST), 31)

        self._text = Text("LMAO", (self.area.centerx, PANEL_PADDING + FONT_SIZE_LARGE//2), fontsize=FONT_SIZE_LARGE)
        self.add_child(self._text)

        self._view_button = self._create_view_button()
        self.add_child(self._view_button)

        self._retry_button = self._create_retry_button()
        self.add_child(self._retry_button)

        self.is_visible = False
        self.can_accept_events = False
    
    def display_end_state(self, state: cm.GameState) -> None:
        """Displays the given game end state on the panel.

        Args:
            state (cm.GameState): The game state to display (CHECKMATE or STALEMATE)
        """
        self.is_visible = True
        self.can_accept_events = True

        match state:
            case cm.GameState.CHECKMATE:
                self._text.text = "Checkmate!"
            case cm.GameState.STALEMATE:
                self._text.text = "Stalemate!"
            case cm.GameState.HALF_MOVE_DRAW:
                self._text.text = "Halfmove draw..."
            case cm.GameState.REPETITION:
                self._text.text = "Draw by repetition..."
            case _:
                self._text.text = "??? This isn't supposed to happen..."

    def set_on_retry_clicked(self, on_click) -> None:
        """Sets the function to be called when the retry button is clicked."""
        self._retry_button.on_click = on_click
    
    def set_on_view_board_clicked(self, on_click) -> None:
        """Sets the function to be called when the view board button is clicked."""
        self._view_button.on_click = on_click