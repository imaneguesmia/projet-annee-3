from .scene_change import SceneId, SceneChange

from ui import UIElement

import pygame
from pygame.event import Event

from abc import ABC, abstractmethod

from typing import final

class Scene(ABC):
    @abstractmethod
    def __init__(self, window_rect: pygame.Rect):
        self.elements: list[UIElement] = []

        self.bg_color = (0, 0, 0)

        self.__surface = pygame.Surface(window_rect.size, flags=pygame.SRCALPHA)

        self.__scene_change: SceneChange | None = None
    
    @final
    def handle_event(self, event: Event) -> None:
        """Handles a pygame event.

        Args:
            event (Event): The event to handle.
        """
        for el in self.elements:
            if el.handle_event(event): break
    
    @final
    def update(self) -> None:
        """Updates all UI elements."""
        for el in self.elements:
            el._recursive_update()
    
    @final
    def draw(self, dest: pygame.Surface) -> None:
        """Draws all UI elements."""
        self.__surface.fill(self.bg_color)

        for el in self.elements:
            el._recursive_draw(self.__surface)
        
        dest.blit(self.__surface, dest.get_rect())

    @final
    def request_scene_change(self, scene_id: SceneId, kwargs: dict[str]) -> None:
        self.__scene_change = SceneChange(scene_id, kwargs)
    @final
    def request_quit(self) -> None:
        self.__scene_change = SceneChange(SceneId.QUIT, {})
    @final
    def requested_scene_change(self) -> SceneChange | None:
        return self.__scene_change