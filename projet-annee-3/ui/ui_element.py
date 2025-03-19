from misc import AttributeWatcher

import pygame

from abc import ABC, abstractmethod
from typing import final, Annotated

class UIElement(AttributeWatcher, ABC):
    def __init__(self, rect: pygame.Rect | None = None):
        super().__init__()

        # The `Rect` occupied by this `UIElement` in relative coordinates (to the destination surface).
        self.dest_rect = rect if rect is not None else pygame.Rect(0, 0, 0, 0)

        self._surface = pygame.Surface(self.dest_rect.size, pygame.SRCALPHA)

        self._children: list[UIElement] = []
        self._parent: UIElement | None = None

        self._hover = False

        # If `True`, then this UI element will be drawn. If not, its drawing will be skipped.
        # This also hides child elements.
        self.is_visible = True
        # If `True`, then this UI element can accept events. If not, all events will be blocked.
        # This also prevents propagation to child elements.
        self.can_accept_events = True

        self.watch("dest_rect", "_hover")
    
    @property
    def area(self) -> pygame.Rect:
        """The size of the `UIElement`, with the corner at `(0, 0)`."""
        return pygame.Rect((0, 0), self.dest_rect.size)
    
    @property
    def absolute_rect(self) -> pygame.Rect:
        """The `Rect` occupied by this `UIElement` in absolute coordinates."""
        if self._parent is not None:
            return self.dest_rect.move(self._parent.absolute_rect.topleft)
        else:
            return self.dest_rect
    
    def point_in_rect(self, point: tuple[int, int]) -> bool:
        """Returns `True` if point is in the absolute bounding `Rect` of this `UIElement`."""
        return self.dest_rect.collidepoint(point)
    
    def add_child(self, child: "UIElement") -> None:
        """Adds the given `UIElement` as a child of this one."""
        self._children.append(child)
        child._parent = self
    
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
    
    def on_userevent(self) -> bool:
        """Method to call on user event.

        Returns:
            bool: `True` if the event was consumed and should no longer be propagated.
        """

    def _will_accept_event(self, event: pygame.event.Event) -> bool:
        """Returns `True` if the event will be accepted by this element."""
        if not self.can_accept_events: return False

        match event.type:
            case pygame.MOUSEBUTTONDOWN:
                accept = self.absolute_rect.collidepoint(event.pos)

            case pygame.MOUSEMOTION:
                self._hover = self.absolute_rect.collidepoint(event.pos)

                # Accept the event if being hovered on OR if hover state changes.
                accept = self._hover or self.is_changed("_hover")
            
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
        if not self._will_accept_event(event): return False

        # Children handle the event first
        consumed = any(el.handle_event(event) for el in self._children)

        if not consumed:
            match event.type:
                case pygame.MOUSEBUTTONDOWN:
                    consumed = self.on_click(event.pos)
                
                case pygame.MOUSEMOTION:
                    if self.is_changed("_hover"):
                        if self._hover:
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
        return self._hover
    
    # -- Updating and drawing -- #

    @final
    def _recursive_update(self) -> None:
        """"""
        self.update()

        # Resize surface if size changed
        if self.is_changed("dest_rect"):
            self._surface = pygame.Surface(self.dest_rect.size, pygame.SRCALPHA)

        for child in self._children:
            child._recursive_update()
        
        self.reset_changed()
    
    @final
    def _recursive_draw(self, dest: pygame.Surface) -> None:
        """"""
        if not self.is_visible: return

        # Clear surface
        self._surface.fill((255, 255, 255, 0))

        self.draw(self._surface)

        for child in self._children:
            child._recursive_draw(self._surface)
        
        dest.blit(self._surface, self.dest_rect)

    @abstractmethod
    def update(self) -> None:
        """Updates the UI element. Called once per cycle, before `UIElement.draw()`."""

    @abstractmethod
    def draw(self, dest: pygame.Surface) -> None:
        """Draws the UI element. Called once per cycle, after `UIElement.update()`.

        Args:
            dest (pygame.Surface): The surface of this UI element. It is the size of this
                                   element's `Rect` and its top-left corner is (0, 0).
        """
