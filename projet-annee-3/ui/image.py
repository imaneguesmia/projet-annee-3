from .ui_element import UIElement

import pygame

from typing import override

class Image(UIElement):
    """"""

    def __init__(self, rect: pygame.Rect, img: pygame.Surface):
        super().__init__(rect)

        self.image = img
    
    @property
    def image(self) -> pygame.Surface:
        return self._image
    @image.setter
    def image(self, img: pygame.Surface):
        self._image = pygame.transform.scale(img, self.area.size)
    
    @override
    def update(self) -> None:
        if self.is_changed("dest_rect"):
            self._image = pygame.transform.scale(self._image, self.area.size)
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.blit(self._image, (0, 0))