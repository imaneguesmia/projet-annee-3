from .ui_element import UIElement
from .button import Button, Text

import pygame

from typing import Any, override

Points = list[tuple[int, int]]

class _CyclingOptionArrow(Button):
    def __init__(self, center: tuple[int, int], arrow_size: int, facing_left: bool):
        """Creates a cycling option arrow.

        Args:
            center (tuple[int, int]): The center of the arrow.
            arrow_size (int): The size of the arrow.
            facing_left (bool): Whether the arrow is facing left instead of right.
        """
        rect = pygame.Rect(0, 0, arrow_size, arrow_size)
        rect.center = center

        super().__init__(rect, "")

        if facing_left:
            pointy_end_x, flat_end_x = self.area.left, self.area.right
        else:
            pointy_end_x, flat_end_x = self.area.right, self.area.left

        self._points = [
            (pointy_end_x, self.area.centery),
            (flat_end_x, self.area.top),
            (flat_end_x, self.area.bottom)
        ]

    @override
    def draw(self, dest: pygame.Surface) -> None:
        pygame.draw.polygon(
            dest, 
            self.hover_color if self.is_hovered else self.color,
            self._points
        )

class CyclingOptions(UIElement):
    def _cycle(self, direction: int) -> None:
        """Cycles through the options in the given direction"""
        self._current_option += direction 
        self._current_option %= len(self._options)

        self._center_text.text = self._options[self._current_option]
    
    def _create_cycle_callback(self, direction: int):
        return lambda _: self._cycle(direction)
    
    def _create_arrows(self, 
        center_width: int,
        arrow_size: int,
        arrow_spacing: int
    ) -> list[_CyclingOptionArrow]:
        """Generates the side arrows.

        Args:
            center_width (int): Width of the central area.
            arrow_size (int): The size of the arrows.
            arrow_spacing (int): The margin between the arrows and the central area.

        Returns:
            list[_CyclingOptionArrow]: The left and right side arrow.
        """
        signs = (-1, 1)
        res: list[_CyclingOptionArrow] = []

        for direction in signs:
            edge = self.area.centerx + direction*(center_width/2)
            arrow_center_x = edge + direction*(arrow_spacing + arrow_size/2)
            
            arrow = _CyclingOptionArrow(
                (arrow_center_x, self.area.centery),
                arrow_size,
                facing_left = (direction == -1)
            )
            
            arrow.on_click = self._create_cycle_callback(direction)

            res.append(arrow)

        return res

    def __init__(self,
        center_rect: pygame.Rect,
        arrow_size: int, arrow_spacing: int,
        options: list[str]
    ):
        """Creates a CyclingOptions UI element.

        Args:
            center_rect (pygame.Rect): Bounding rectangle for the central area, where the selected option is displayed.
            arrow_size (int): The size of the arrow buttons used for selection.
            arrow_spacing (int): The margin between the arrows and the central area.
            options (list[str]): The list of available options.
        """
        rect = pygame.Rect(
            0, 0,
            center_rect.width + 2*(arrow_size + arrow_spacing),
            max(center_rect.height, arrow_size)
        )
        rect.center = center_rect.center

        super().__init__(rect)

        self.center_color = (0, 100, 0)

        self._center_rect = center_rect
        self._center_rect.center = self.area.center

        arrows = self._create_arrows(center_rect.width, arrow_size, arrow_spacing)
        self._left_arrow, self._right_arrow = arrows
        self.add_child(self._left_arrow)
        self.add_child(self._right_arrow)

        if (len(options) == 0): 
            raise ValueError("CyclingOptions must have at least 1 option.")
        
        self._options = options
        self._current_option = 0

        self._center_text = Text(options[0], self.area.center)
        self.add_child(self._center_text)
    
    @property
    def center_text_color(self) -> pygame.Color:
        return self._center_text.color
    @center_text_color.setter
    def center_text_color(self, color: pygame.Color) -> None:
        self._center_text.color = color
    
    @property
    def center_text_font(self) -> pygame.font.Font:
        return self._center_text.font
    @center_text_font.setter
    def center_text_font(self, font: pygame.font.Font) -> None:
        self._center_text.font = font
    
    @property
    def arrow_color(self) -> pygame.Color:
        return self._left_arrow.color
    @arrow_color.setter
    def arrow_color(self, color: pygame.Color) -> None:
        self._left_arrow.color = color
        self._right_arrow.color = color

    @property
    def arrow_hover_color(self) -> pygame.Color:
        return self._left_arrow.hover_color
    @arrow_color.setter
    def arrow_hover_color(self, color: pygame.Color) -> None:
        self._left_arrow.hover_color = color
        self._right_arrow.hover_color = color

    @override
    def update(self) -> None:
        pass

    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.fill(self.center_color, self._center_rect)
    
    def get_selected_option(self) -> int:
        """Returns the index of the currently selected option."""
        return self._current_option
