from misc import AttributeWatcher

import pygame

from abc import ABC, abstractmethod
from typing import final, Annotated

class UIElement(AttributeWatcher, ABC):
    def __init__(self, rect: pygame.Rect | None = None):
        super().__init__()

        # The `Rect` occupied by this `UIElement` in relative coordinates (to the destination surface).
        self.dest_rect = rect if rect is not None else pygame.Rect(0, 0, 0, 0)

        self.__surface = pygame.Surface(self.dest_rect.size, pygame.SRCALPHA)

        self.__children: list[UIElement] = []
        self.__parent: UIElement | None = None

        self.__hover = False

        self.watch("dest_rect", "_UIElement__hover")
    
    @property
    def area(self) -> pygame.Rect:
        """The size of the `UIElement`, with the corner at `(0, 0)`."""
        return pygame.Rect((0, 0), self.dest_rect.size)
    
    @property
    def absolute_rect(self) -> pygame.Rect:
        """The `Rect` occupied by this `UIElement` in absolute coordinates."""
        if self.__parent is not None:
            return self.dest_rect.move(self.__parent.absolute_rect.topleft)
        else:
            return self.dest_rect
    
    def point_in_rect(self, point: tuple[int, int]) -> bool:
        """Returns `True` if point is in the absolute bounding `Rect` of this `UIElement`."""
        return self.dest_rect.collidepoint(point)
    
    def add_child(self, child: "UIElement") -> None:
        """Adds the given `UIElement` as a child of this one."""
        self.__children.append(child)
        child.__parent = self
    
    # -- Event handling -- #
    
    def on_click(self, point: tuple[int, int]) -> bool:
        """Method to call on click event.

        Args:
            point (tuple[int, int]): Coordinates of the click.

        Returns:
            bool: `True` if the event was consumed and should no longer be propagated.
        """
        return False
    
    def on_hover(self, point: tuple[int, int]) -> bool:
        """Method to call on hover event.

        Args:
            point (tuple[int, int]): Coordinates of the mouse.

        Returns:
            bool: `True` if the event was consumed and should no longer be propagated.
        """
        return False

    def on_mouseenter(self, point: tuple[int, int]) -> bool:
        """Method to call on mouse enter event.

        Args:
            point (tuple[int, int]): Coordinates of the mouse.

        Returns:
            bool: `True` if the event was consumed and should no longer be propagated.
        """
        return False
    
    def on_mouseleave(self, point: tuple[int, int]) -> bool:
        """Method to call on mouse leave event.

        Args:
            point (tuple[int, int]): Coordinates of the mouse.

        Returns:
            bool: `True` if the event was consumed and should no longer be propagated.
        """
        return False

    def __will_accept_event(self, event: pygame.event.Event) -> bool:
        """Returns `True` if the event will be accepted by this element."""
        match event.type:
            case pygame.MOUSEBUTTONDOWN:
                accept = self.absolute_rect.collidepoint(event.pos)

            case pygame.MOUSEMOTION:
                self.__hover = self.absolute_rect.collidepoint(event.pos)

                # Accept the event if being hovered on OR if hover state changes.
                accept = self.__hover or self.is_changed("_UIElement__hover")
            
            case _:
                accept = False
        
        return accept

    @final
    def handle_event(self, event: pygame.event.Event) -> bool:
        """Handle pygame event

        Args:
            event (pygame.event.Event): The event to handle.

        Returns:
            bool: `True` if the event was consumed and should no longer be propagated.
        """
        if not self.__will_accept_event(event): return False

        # Children handle the event first
        consumed = any(el.handle_event(event) for el in self.__children)

        if not consumed:
            match event.type:
                case pygame.MOUSEBUTTONDOWN:
                    consumed = self.on_click(event.pos)
                
                case pygame.MOUSEMOTION:
                    if self.is_changed("_UIElement__hover"):
                        if self.__hover:
                            consumed = self.on_mouseenter(event.pos)
                        else:
                            consumed = self.on_mouseleave(event.pos)
                        
                        self.reset_changed()
                    
                    consumed |= self.on_hover(event.pos)
                case _:
                    consumed = False
        
        return consumed
    
    @property
    def is_hovered(self) -> bool:
        return self.__hover
    
    # -- Updating and drawing -- #

    @final
    def _recursive_update(self) -> None:
        """"""
        self.update()

        # Resize surface if size changed
        if self.is_changed("dest_rect"):
            self.__surface = pygame.Surface(self.dest_rect.size, pygame.SRCALPHA)

        for child in self.__children:
            child._recursive_update()
        
        self.reset_changed()
    
    @final
    def _recursive_draw(self, dest: pygame.Surface) -> None:
        """"""
        # Clear surface
        self.__surface.fill((255, 255, 255, 0))

        self.draw(self.__surface)

        for child in self.__children:
            child._recursive_draw(self.__surface)
        
        dest.blit(self.__surface, self.dest_rect)

    @abstractmethod
    def update(self) -> None: ...

    @abstractmethod
    def draw(self, dest: pygame.Surface) -> None: ...
