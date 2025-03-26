from .board_base import BoardBase

from .puzzles import Highlight, HighlightType

import cpp_chess as cm

import pygame

from typing import override

TR_RED = (255, 0, 0, 128)

ARROW_HEAD_WIDTH = 30
ARROW_HEAD_HEIGHT = 50
ARROW_THICKNESS = 15

CIRCLE_THICKNESS = 7

class BoardHighlighter(BoardBase):
    """UI element to display the chessboard. Essentially the View and Controller in an MVC model."""

    def __init__(self, rect: pygame.Rect):
        super().__init__(rect)

        self._highlights = pygame.Surface(self.area.size, pygame.SRCALPHA)

        # pygame.draw.circle(self._highlights, TR_RED, self._highlights.get_rect().center, 100)
        # self.add_arrow()
    
    @override
    def update(self) -> None:
        pass

    @override
    def draw(self, dest: pygame.Surface) -> None:
        """Dessine le plateau avec les pièces et les coups légaux."""
        dest.blit(self._highlights, (0, 0))
    
    def _center_tl(self, tl: tuple[int, int]) -> tuple[int, int]:
        """Centers the top-left of a square."""
        return (tl[0] + self.square_size//2, tl[1] + self.square_size//2)

    def _clear_highlights(self) -> None:
        """Clears all highlights."""
        self._highlights.fill((0, 0, 0, 0))
    
    def _add_arrow(self, fr: cm.Position, to: cm.Position) -> None:
        """Draws an arrow from a position to a position."""
        fr = self.square_to_coordinates(fr)
        to = self.square_to_coordinates(to)

        fr_vector = pygame.Vector2(self._center_tl(fr))
        to_vector = pygame.Vector2(self._center_tl(to))

        d = to_vector - fr_vector
        angle = d.angle_to((0.0, 1.0))
        arrow_length = d.length()

        # Arrow points if it was completely vertical.
        points = [
            pygame.Vector2(+ARROW_THICKNESS/2, 0),
            pygame.Vector2(+ARROW_THICKNESS/2, arrow_length-ARROW_HEAD_HEIGHT),
            pygame.Vector2(+ARROW_HEAD_WIDTH, arrow_length-ARROW_HEAD_HEIGHT),
            pygame.Vector2(0, arrow_length),
            pygame.Vector2(-ARROW_HEAD_WIDTH, arrow_length-ARROW_HEAD_HEIGHT),
            pygame.Vector2(-ARROW_THICKNESS/2, arrow_length-ARROW_HEAD_HEIGHT),
            pygame.Vector2(-ARROW_THICKNESS/2, 0),
        ]

        # Rotate and place the arrow.
        for p in points:
            p.rotate_ip(-angle)
            p += fr_vector
        
        pygame.draw.polygon(self._highlights, TR_RED, points)
    
    def _add_square(self, position: cm.Position) -> None:
        """Draws a highlight around a square."""
        tl = self.square_to_coordinates(position)
        center = self._center_tl(tl)

        pygame.draw.circle(
            self._highlights,
            TR_RED,
            center, self.square_size//2,
            CIRCLE_THICKNESS
        )
    
    def highlight(self, highlights: list[Highlight]) -> None:
        """Processes and displays the list of highlights."""
        for highlight in highlights:
            match highlight.type:
                case HighlightType.ARROW:
                    self._add_arrow(highlight.position, highlight.target)
                case HighlightType.SQUARE:
                    self._add_square(highlight.position)
                case HighlightType.CLEAR:
                    self._clear_highlights()