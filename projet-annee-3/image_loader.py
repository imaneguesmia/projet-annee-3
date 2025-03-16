import cpp_chess as cm

import pygame

from enum import Enum

IMG_DIR = "resources/images/"

class PanelTheme(Enum):
    TEST = 0
    LEFT = 1
    INSET = 2

class ImageLoader:
    def __init__(self, square_size: float):
        self.square_size = square_size

        self._pieces: dict[str, pygame.Surface] = {}
        self._tiles: list[pygame.Surface] = []
        self._select: list[list[pygame.Surface]] = []
        self._panels: list[pygame.Surface] = []

        self._load_pieces()
        self._load_tiles()
        self._load_select()
        self._load_panels()

    def _load_square_image(self, path: str) -> pygame.Surface:
        img = pygame.image.load(f"{IMG_DIR}{path}")

        return pygame.transform.scale(img, (self.square_size, self.square_size))
    
    def _load_pieces(self) -> None:
        for i, piece in enumerate("PpNnBbRrQqKk"):
            self._pieces[piece] = self._load_square_image(f"piece{i % 2}{i // 2}.png")
    
    def _load_tiles(self) -> None:
        for i in range(2):
            self._tiles.append(self._load_square_image(f"tiles{i}.png"))
    
    def _load_select(self) -> None:
        for i in range(2):
            self._select.append([])

            for j in range(3):
                self._select[i].append(self._load_square_image(f"select{i}{j}.png"))
    
    def _load_panels(self) -> None:
        for i in range(3):
            self._panels.append(pygame.image.load(f"{IMG_DIR}panels{i}.png"))
    
    # -- Get specific sprites -- #

    def piece_sprite(self, piece: cm.Piece) -> pygame.Surface:
        return self._pieces[piece.fen()]

    def tile_sprite(self, index: int) -> pygame.Surface:
        return self._tiles[index]

    def selected_sprite(self, tile_index: int) -> pygame.Surface:
        return self._select[tile_index][0]

    def attacked_sprite(self, tile_index: int, is_capture: bool) -> pygame.Surface:
        return self._select[tile_index][2 if is_capture else 1]

    def panel(self, theme: PanelTheme) -> pygame.Surface:
        return self._panels[theme.value]

IMAGES: ImageLoader