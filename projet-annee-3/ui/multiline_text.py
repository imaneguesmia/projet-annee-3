from .ui_element import UIElement

from .font import FONT_PATH
from .text_align import TextAlign

import pygame

from typing import override

class MultilineText(UIElement):
    def __init__(self,
            rect: pygame.Rect,
            text: str,
            align: TextAlign = TextAlign.LEFT,
            fontsize=120,
            line_spacing=10,
            color=pygame.color.Color(0x000000FF),
            bg_color=pygame.color.Color(0x00000000)
        ):
        super().__init__(rect)

        self.text = text
        self.font = pygame.font.Font(FONT_PATH, fontsize)
    
        self.color = color
        self.bg_color = bg_color

        self.align = align
        self.line_spacing = line_spacing

        self._update_surfaces()

        self.watch("text", "font", "color", "bg_color", "align", "line_spacing")

    def _update_surfaces(self) -> None:
        self._alpha_surface = pygame.Surface(self.area.size, pygame.SRCALPHA)
        self._alpha_surface.fill(self.bg_color)

        self._text_surface = pygame.Surface(self.area.size, pygame.SRCALPHA)
        self._text_surface.blit(self._alpha_surface, (0, 0))

        y = 0

        for line in self.text.splitlines():
            text = self.font.render(line, True, self.color)

            match self.align:
                case TextAlign.LEFT:
                    x = 0
                case TextAlign.CENTER:
                    x = self.area.centerx - text.get_width()//2
                case TextAlign.RIGHT:
                    x = self.area.right - text.get_width()

            self._text_surface.blit(text, (x, y))

            y += self.font.get_linesize() + self.line_spacing
    
    @override
    def update(self) -> None:
        if self.is_changed("text") or self.is_changed("font") or \
           self.is_changed("color") or self.is_changed("bg_color") or \
           self.is_changed("align") or self.is_changed("dest_rect"):
            self._update_surfaces()
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.blit(self._text_surface, (0, 0))