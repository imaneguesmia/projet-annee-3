import image_loader as img

from ui import Button, NinepatchPanel, Image, Text
from ui.colors import *

import cpp_chess as cm

import pygame

from typing import override

# PANEL_WIDTH = 400
# PANEL_HEIGHT = 100
# PANEL_PADDING = 8

# BUTTON_PADDING = 8
# BUTTON_SIZE = PANEL_WIDTH/4 - BUTTON_PADDING*2

PANEL_WIDTH = 600
PANEL_HEIGHT = 400
PANEL_PADDING = 20

FONT_SIZE = 60

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

# class PromotionButton(Button):
#     def __init__(self, position: tuple[int, int], p_type: cm.PType, model: ChessModel):
#         button_rect = pygame.Rect(position, (BUTTON_SIZE, BUTTON_SIZE))
#         piece = cm.Piece(p_type, cm.Player.White)

#         super().__init__(button_rect, "")

#         self._piece = piece
#         self._model = model

#         self.color = (0, 0, 0, 0)
#         self.hover_color = (255, 255, 255, 100)

#         image_rect = button_rect.copy()
#         image_rect.topleft = (0, 0)
#         self._image = Image(image_rect, img.IMAGES.piece_sprite(self._piece))

#         self.add_child(self._image)
    
#     @override
#     def on_click(self, _) -> bool:
#         print(self._piece.fen())

#         self._model.do_current_promotion(self._piece.get_type())

#         return True
    
#     def set_player(self, player: cm.Player) -> None:
#         """"""
#         self._piece = cm.Piece(self._piece.get_type(), player)

#         self._image.image = img.IMAGES.piece_sprite(self._piece)

        # self._text.text = self._piece.fen()

class GameEndPanel(NinepatchPanel):
    def _create_button(self, button_rect: pygame.Rect, text: str) -> Button:
        """Creates a button to be displayed on this panel."""
        button = Button(button_rect, text)
        button.color = SAND_COLOR
        button.hover_color = SAND_HOVER
        button.text_color = BLACK
        button.text_font = pygame.font.Font(None, 50)

        return button
    
    def _create_quit_button(self) -> Button:
        """Creates the 'Quit' button."""
        rect = pygame.Rect(
            (self.area.width*3)//4 - BUTTON_WIDTH//2,
            self.area.height - PANEL_PADDING - BUTTON_HEIGHT,
            BUTTON_WIDTH, BUTTON_HEIGHT
        )

        button = self._create_button(rect, "Quit")

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

        self._text = Text("LMAO", (self.area.centerx, PANEL_PADDING + FONT_SIZE//2), fontsize=FONT_SIZE)
        self.add_child(self._text)

        self._view_button = self._create_view_button()
        self.add_child(self._view_button)

        self._quit_button = self._create_quit_button()
        self.add_child(self._quit_button)

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
            case _:
                self._text.text = "??? This isn't supposed to happen..."

    # def on_quit_clicked(self) -> None: ...
    # def on_view_board_clicked(self) -> None: ...

    def set_on_quit_clicked(self, on_click) -> None:
        """Sets the function to be called when the quit button is clicked."""
        self._quit_button.on_click = on_click
    
    def set_on_view_board_clicked(self, on_click) -> None:
        """Sets the function to be called when the view board button is clicked."""
        self._view_button.on_click = on_click