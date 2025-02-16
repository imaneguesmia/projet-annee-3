import cpp_chess as cm

import pygame

IMG_DIR = "projet-annee-3/images/"

class ImageLoader:
    def __init__(self, square_size: float):
        self.square_size = square_size

        self.__pieces: dict[str, pygame.Surface] = {}
        self.__tiles: list[pygame.Surface] = []
        self.__select: list[list[pygame.Surface]] = []

        self.__load_pieces()
        self.__load_tiles()
        self.__load_select()

    def __load_square_image(self, path: str) -> pygame.Surface:
        img = pygame.image.load(f"{IMG_DIR}{path}")

        return pygame.transform.scale(img, (self.square_size, self.square_size))
    
    def __load_pieces(self) -> None:
        for i, piece in enumerate("PpNnBbRrQqKk"):
            self.__pieces[piece] = self.__load_square_image(f"piece{i % 2}{i // 2}.png")
    
    def __load_tiles(self) -> None:
        for i in range(2):
            self.__tiles.append(self.__load_square_image(f"tiles{i}.png"))
    
    def __load_select(self) -> None:
        for i in range(2):
            self.__select.append([])

            for j in range(3):
                self.__select[i].append(self.__load_square_image(f"select{i}{j}.png"))
    
    # -- Get specific sprites -- #

    def piece_sprite(self, piece: cm.Piece) -> pygame.Surface:
        return self.__pieces[piece.fen()]

    def tile_sprite(self, index: int) -> pygame.Surface:
        return self.__tiles[index]

    def selected_sprite(self, tile_index: int) -> pygame.Surface:
        return self.__select[tile_index][0]

    def attacked_sprite(self, tile_index: int, is_capture: bool) -> pygame.Surface:
        return self.__select[tile_index][2 if is_capture else 1]


__all__ = ["ImageLoader"]