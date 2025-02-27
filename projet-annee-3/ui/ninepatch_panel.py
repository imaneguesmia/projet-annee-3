from .panel import Panel

import pygame

from typing import override

EDGES = [(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)]

class NinepatchPanel(Panel):
    """"""

    def __init__(self, rect: pygame.Rect, src: pygame.Surface, edge_size: int = 16):
        super().__init__(rect, (0, 0, 0, 0))

        self.src = src
        self.edge_size = edge_size
    
        self._panel = self._generate_patched()

        self.watch("src", "edge_size")
    
    def _generate_patch_rects(self, e: int, w: int, h: int) -> list[pygame.Rect]:
        """"""

        ew, eh = w-e*2, h-e*2
        fw, fh = w-e, h-e

        return map(
            pygame.Rect,
            [(0, 0, e, e), (e, 0, ew, e), (fw, 0, e, e),
            (0, e, e, eh), (e, e, ew, eh), (fw, e, e, eh),
            (0, fh, e, e), (e, fh, ew, e), (fw, fh, e, e)]
        )

    def _generate_patched(self) -> pygame.Surface:
        """"""
        src_rects = self._generate_patch_rects(self.edge_size, self.src.get_width(), self.src.get_height())
        dest_rects = self._generate_patch_rects(self.edge_size, self.area.width, self.area.height)

        patched = pygame.Surface(self.area.size, flags=pygame.SRCALPHA)

        for src_rect, dest_rect in zip(src_rects, dest_rects):
            src_patch = self.src.subsurface(src_rect)
            stretched = pygame.transform.smoothscale(src_patch, dest_rect.size)
            patched.blit(stretched, dest_rect)
        
        return patched
    
    @override
    def update(self) -> None:
        if self.is_changed("src") or self.is_changed("edge_size"):
            self._panel = self._generate_patched()
    
    @override
    def draw(self, dest: pygame.Surface) -> None:
        dest.blit(self._panel, (0, 0))