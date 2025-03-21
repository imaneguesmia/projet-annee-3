from ui import NinepatchPanel, Text, TextAlign
from ui.font import FONT_SIZE_LARGE

import image_loader as img

import pygame

from abc import ABC

HEADER_FONT_SIZE = FONT_SIZE_LARGE

PANEL_PADDING_X = 20
PANEL_PADDING_Y = 5

class TitlePanel(NinepatchPanel, ABC):
    def __init__(self, rect: pygame.Rect, text: str):
        super().__init__(rect, img.IMAGES.panel(img.PanelTheme.TEST), 31)

        anchor = (self.area.centerx, HEADER_FONT_SIZE//2 + PANEL_PADDING_Y)

        self._title = Text(text, anchor, align=TextAlign.CENTER, fontsize=HEADER_FONT_SIZE)
        self.add_child(self._title)

    def set_title(self, text: str) -> None:
        """Sets the title of the main menu panel to the given string."""
        self._title.text = text