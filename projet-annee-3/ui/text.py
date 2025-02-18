from .ui_element import UIElement

import pygame

from typing import override

class Text(UIElement):
    def __init__(self, 
            text: str, center: tuple[int, int], 
            fontsize=120, 
            color=pygame.color.Color(0x000000FF),
            bg_color=pygame.color.Color(0x00000000)
        ):
        super().__init__()

        self.text = text
        self.font = pygame.font.Font(None, fontsize)

        self.color = color
        self.bg_color = bg_color

        self.center = center

        self.__update_surfaces()

        self.watch("text", "font", "color", "bg_color")
    
    def __update_surfaces(self) -> None:
        self.__alpha_surface = pygame.Surface(self.area.size, pygame.SRCALPHA)
        self.__alpha_surface.fill(self.bg_color)

        text = self.font.render(self.text, True, self.color)

        self.dest_rect = text.get_rect(center=self.center)

        self.__text_surface = pygame.Surface(self.area.size, pygame.SRCALPHA)

        self.__text_surface.blit(self.__alpha_surface, (0, 0))
        self.__text_surface.blit(text, (0, 0))

    @override
    def update(self) -> None:
        if self.is_changed("text") or self.is_changed("font") or \
           self.is_changed("color") or self.is_changed("bg_color"):
            self.__update_surfaces()
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.blit(self.__text_surface, (0, 0))