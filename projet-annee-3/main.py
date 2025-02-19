import cpp_chess as cm
import pygame
from chess_board import ChessBoard

# Initialisation de Pygame
pygame.init()
window_size = 800
fenetre = pygame.display.set_mode((window_size, window_size))
pygame.display.set_caption("Aqua Chess")

# Initialisation des objets ChessGame et ChessBoard
# chess_game = cm.Game()
# chess_board = ChessBoard(fenetre, window_size, chess_game)

from scenes import SceneManager, SceneId

# Boucle principale
running = True
scene_manager = SceneManager(fenetre, SceneId.MAINMENU)

while running:
    running = scene_manager.update_loop()

pygame.quit()



