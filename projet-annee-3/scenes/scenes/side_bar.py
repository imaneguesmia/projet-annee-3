import image_loader as img

from ..data_transfer.player_type import PlayerType

from ui import Button, NinepatchPanel, Image, Text, TextAlign
from ui.colors import *

import cpp_chess as cm

import pygame

HEADER_FONT_SIZE = 40

BUTTON_WIDTH = 200
BUTTON_HEIGHT = 80
BUTTON_MARGIN = 20

PANEL_WIDTH = 400
PANEL_PADDING = 20

EVAL_PANEL_HEIGHT = 200
EVAL_PANEL_V_PADDING = 5
EVAL_PANEL_H_PADDING = 10
EVAL_PANEL_SPACING = 20

class SideBar(NinepatchPanel):
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
    
    def _player_type_to_string(self, type: PlayerType) -> str:
        """Returns the string representation of the given player type."""
        match type:
            case PlayerType.HUMAN:
                return "Human"
            case PlayerType.MINIMAX:
                return "Minimax"
            case PlayerType.NEURAL_NET:
                return "Neural Net"
    
    def _create_evaluation_panel(self, player: cm.Player, y_offset: int) -> tuple[NinepatchPanel, Text, Text]:
        """Creates the panel displaying position evaluation by the AI.
        
        Returns:
          - Tuple containing the panel, the evaluation text and the win state text.
        """
        rect = pygame.Rect(
            PANEL_PADDING, PANEL_PADDING + y_offset,
            PANEL_WIDTH - PANEL_PADDING*2, EVAL_PANEL_HEIGHT
        )

        panel = NinepatchPanel(rect, img.IMAGES.panel(img.PanelTheme.INSET), 31)

        # header_text = "White" if player == cm.Player.White else "Black"
        # player_type = self._w
        if player == cm.Player.White:
            header_text = "White"
            player_type = self._white_player
        else:
            header_text = "Black"
            player_type = self._black_player


        panel.add_child(
            Text(
                f"{header_text} : {self._player_type_to_string(player_type)}", 
                (panel.area.centerx, HEADER_FONT_SIZE//2 + EVAL_PANEL_V_PADDING),
                fontsize=HEADER_FONT_SIZE,
                color="blue"
            )
        )

        eval_label = Text(
            "Evaluation = ",
            (EVAL_PANEL_H_PADDING, HEADER_FONT_SIZE//2 + HEADER_FONT_SIZE + EVAL_PANEL_V_PADDING),
            align=TextAlign.LEFT,
            fontsize=HEADER_FONT_SIZE,
            color="blue"
        )
        eval_text = Text(
            "--",
            (eval_label.dest_rect.right, HEADER_FONT_SIZE//2 + HEADER_FONT_SIZE + EVAL_PANEL_V_PADDING),
            align=TextAlign.LEFT,
            fontsize=HEADER_FONT_SIZE,
            color="blue"
        )
        win_state_text = Text(
            "???",
            (EVAL_PANEL_H_PADDING, HEADER_FONT_SIZE//2 + HEADER_FONT_SIZE*2 + EVAL_PANEL_V_PADDING),
            align=TextAlign.LEFT,
            fontsize=HEADER_FONT_SIZE,
            color="blue"
        )

        panel.add_child(eval_label)
        panel.add_child(eval_text)
        panel.add_child(win_state_text)

        return panel, eval_text, win_state_text

    def __init__(self, window_rect: pygame.Rect, player_types: tuple[PlayerType, PlayerType]):
        panel_rect = pygame.Rect(
            window_rect.width - PANEL_WIDTH, 0,
            PANEL_WIDTH,
            window_rect.height
        )

        super().__init__(panel_rect, img.IMAGES.panel(img.PanelTheme.LEFT), 31)

        self._white_player = player_types[0]
        self._black_player = player_types[1]

        self._eval_panel_1, self._eval_text_1, self._win_state_text_1 = self._create_evaluation_panel(
            cm.Player.White, 
            0
        )
        self.add_child(self._eval_panel_1)

        self._eval_panel_2, self._eval_text_2, self._win_state_text_2 = self._create_evaluation_panel(
            cm.Player.Black,
            EVAL_PANEL_HEIGHT + EVAL_PANEL_SPACING
        )
        self.add_child(self._eval_panel_2)
    
    def set_evaluation(self, value: int | None, player: cm.Player) -> None:
        """Sets the displayed evaluation value for the current position."""
        if player == cm.Player.White:
            eval_text = self._eval_text_1
            win_state_text = self._win_state_text_1
        else:
            eval_text = self._eval_text_2
            win_state_text = self._win_state_text_2

        # eval_text.text = str(value) if value is not None else "--"
        if value is None:
            eval_text.text = "--"
        else:
            eval_text.text = str(value)

            if value == 0:
                win_state_text.text = ""
            elif value > 0:
                win_state_text.text = "(Winning!)"
            else:
                win_state_text.text = "(Losing...)"
            

    def set_on_quit_clicked(self, on_click) -> None:
        """Sets the function to be called when the quit button is clicked."""
        self._quit_button.on_click = on_click