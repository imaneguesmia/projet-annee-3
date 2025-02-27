from .ui_element import UIElement

import pygame

from typing import override

class Panel(UIElement):
    """Simple panel element to serve as a backdrop for other elements."""

    def __init__(self, rect: pygame.Rect, bg_color: pygame.color.Color = (255, 255, 255)):
        super().__init__(rect)

        self.bg_color = bg_color
    
    @override
    def update(self) -> None:
        pass
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.fill(self.bg_color)