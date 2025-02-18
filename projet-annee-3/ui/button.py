from .ui_element import UIElement
from .text import Text

import pygame

from typing import override

class Button(UIElement):
    def __init__(self, rect: pygame.Rect, text: str):
        super().__init__(rect)

        self.__text = Text(text, self.area.center, fontsize=50)
        self.add_child(self.__text)

        self.color = (100, 0, 0)
        self.hover_color = (200, 0, 0)

        self.__hover = False
    
    @property
    def text_color(self) -> pygame.Color:
        return self.__text.color
    @text_color.setter
    def text_color(self, color: pygame.Color) -> None:
        self.__text.color = color
    
    @property
    def text_font(self) -> pygame.font.Font:
        return self.__text.font
    @text_font.setter
    def text_font(self, font: pygame.font.Font) -> None:
        self.__text.font = font

    @override
    def update(self) -> None:
        pass
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.fill(self.hover_color if self.is_hovered else self.color)

    # @override
    # def on_hover(self, point: tuple[int, int]) -> bool:
    #     self.__hover = True
    #     return False