from .ui_element import UIElement
from .text import Text

import pygame

from typing import override

class Button(UIElement):
    def __init__(self, rect: pygame.Rect, text: str):
        super().__init__(rect)

        self._text = Text(text, self.area.center, fontsize=50)
        self.add_child(self._text)

        self.color = (100, 0, 0)
        self.hover_color = (200, 0, 0)

        self._hover = False
    
    @property
    def text_color(self) -> pygame.Color:
        return self._text.color
    @text_color.setter
    def text_color(self, color: pygame.Color) -> None:
        self._text.color = color
    
    @property
    def text_font(self) -> pygame.font.Font:
        return self._text.font
    @text_font.setter
    def text_font(self, font: pygame.font.Font) -> None:
        self._text.font = font

    @override
    def update(self) -> None:
        pass
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.fill(self.hover_color if self.is_hovered else self.color)

    # @override
    # def on_hover(self, point: tuple[int, int]) -> bool:
    #     self._hover = True
    #     return False