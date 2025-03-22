from .button import Button

import pygame

from typing import override

class Checkbox(Button):
    def _centered_rect_helper(self, size: int) -> pygame.Rect:
        """Creates and returns a rect centered on this element with the given size."""
        rect = pygame.Rect(0, 0, size, size)
        rect.center = self.area.center

        return rect

    def __init__(self, center: tuple[int, int], size: int, border_size: int, checked: bool = False):
        rect = pygame.Rect(0, 0, size, size)
        rect.center = center

        super().__init__(rect, "")

        self._border_size = border_size

        self._outer_rect = self.area
        self._mid_rect = self._centered_rect_helper(size - border_size)
        self._inner_rect = self._centered_rect_helper(size - border_size*2)

        self._checked = checked

        self.checked_color = "green"
    
    @override
    def on_click(self, _) -> bool:
        self._checked = not self._checked

        return True
    
    @override
    def draw(self, dest_rect: pygame.Surface) -> None:
        dest_rect.fill("black", self._outer_rect)
        dest_rect.fill((0, 0, 0, 0), self._mid_rect)

        if self._checked:
            dest_rect.fill(self.checked_color, self._inner_rect)
    
    @property
    def checked(self) -> bool:
        return self._checked