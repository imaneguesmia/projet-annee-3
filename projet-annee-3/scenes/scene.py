from .scene_change import SceneId, SceneChange

from ui import UIElement

import pygame
from pygame.event import Event

from abc import ABC, abstractmethod

from typing import final

class Scene(ABC):
    @abstractmethod
    def __init__(self, window_rect: pygame.Rect, bg_image_path: str=None):
        self.window_rect = window_rect

        self.elements: list[UIElement] = []

        self.bg_color = (0, 0, 0)

        self._surface = pygame.Surface(window_rect.size, flags=pygame.SRCALPHA)

        self._scene_change: SceneChange | None = None

        self._bg_image = None
        if bg_image_path:
            raw_bg_image = pygame.image.load(bg_image_path).convert()

            # Redimensionner l'image à la taille réelle de la fenêtre
            self._bg_image = pygame.transform.scale(raw_bg_image, window_rect.size)

            print(f"[Scene] Background loaded and scaled to {window_rect.size}")  # Debug info


    
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

        """Draw background."""
        if self._bg_image:
            dest.blit(self._bg_image, (0,0))  
        else:
            self._surface.fill(self.bg_color)  

        """Draws all UI elements."""

        for el in self.elements:
            el._recursive_draw(self._surface)
        
        dest.blit(self._surface, dest.get_rect())

    @final
    def request_scene_change(self, scene_id: SceneId, kwargs: dict[str]) -> None:
        self._scene_change = SceneChange(scene_id, kwargs)
    @final
    def request_quit(self) -> None:
        self._scene_change = SceneChange(SceneId.QUIT, {})
    @final
    def requested_scene_change(self) -> SceneChange | None:
        return self._scene_change