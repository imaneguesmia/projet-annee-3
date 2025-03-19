from .ui_element import UIElement

from .font import FONT_PATH

import pygame

from typing import override
from enum import Enum, auto

class TextAlign(Enum):
    LEFT = auto()
    CENTER = auto()
    RIGHT = auto()

class Text(UIElement):
    def __init__(self, 
            text: str, anchor: tuple[int, int], 
            align: TextAlign = TextAlign.CENTER,
            fontsize=120,
            color=pygame.color.Color(0x000000FF),
            bg_color=pygame.color.Color(0x00000000)
        ):
        super().__init__()

        self.text = text
        self.font = pygame.font.Font(FONT_PATH, fontsize)

        self.color = color
        self.bg_color = bg_color

        self.anchor = anchor
        self.align = align

        self._update_surfaces()

        self.watch("text", "anchor", "align", "font", "color", "bg_color")
    
    def _update_surfaces(self) -> None:
        self._alpha_surface = pygame.Surface(self.area.size, pygame.SRCALPHA)
        self._alpha_surface.fill(self.bg_color)

        text = self.font.render(self.text, True, self.color)

        self.dest_rect = text.get_rect()
        
        match self.align:
            case TextAlign.LEFT:
                self.dest_rect.left = self.anchor[0]
                self.dest_rect.centery = self.anchor[1]
            case TextAlign.CENTER:
                self.dest_rect.center = self.anchor
            case TextAlign.RIGHT:
                self.dest_rect.right = self.anchor[0]
                self.dest_rect.centery = self.anchor[1]

        self._text_surface = pygame.Surface(self.area.size, pygame.SRCALPHA)

        self._text_surface.blit(self._alpha_surface, (0, 0))
        self._text_surface.blit(text, (0, 0))

    @override
    def update(self) -> None:
        if self.is_changed("text") or self.is_changed("font") or \
           self.is_changed("color") or self.is_changed("bg_color") or \
           self.is_changed("anchor") or self.is_changed("align"):
            self._update_surfaces()
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.blit(self._text_surface, (0, 0))