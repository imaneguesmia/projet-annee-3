import cpp_chess as cm
import pygame
from chess_board import ChessBoard

BOARD_SIZE = 800

# Initialisation de Pygame
pygame.init()
board_size = BOARD_SIZE
fenetre = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)  # Met automatiquement la résolution native
SCREEN_WIDTH, SCREEN_HEIGHT = fenetre.get_size()  # Récupère la taille de l'écran
pygame.display.set_caption("Aqua Chess")

# Initialisation des objets ChessGame et ChessBoard
chess_game = cm.Game()
board_x = (SCREEN_WIDTH - BOARD_SIZE) // 2  
board_y = (SCREEN_HEIGHT - BOARD_SIZE) // 2  
rect = pygame.Rect(board_x, board_y, BOARD_SIZE, BOARD_SIZE)
chess_board = ChessBoard(rect, board_size, chess_game)

from scenes import SceneManager, SceneId
import image_loader as img

# Load images globally
img.IMAGES = img.ImageLoader(board_size // 8)

# Boucle principale
running = True
scene_manager = SceneManager(fenetre, SceneId.MAINMENU) 

while running:
    running = scene_manager.update_loop()

pygame.quit()



