import cpp_chess as cm
import pygame
from chess_board import ChessBoard
from scenes import SceneManager, SceneId
import image_loader as img

# ------------------------------
# Initialisation de Pygame
# ------------------------------
pygame.init()

# ✅ Fenêtre en FULLSCREEN sur l'écran actif
fenetre = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
SCREEN_WIDTH, SCREEN_HEIGHT = fenetre.get_size()

pygame.display.set_caption("Beluga Engine")

# ------------------------------
# Constantes de l'échiquier
# ------------------------------
BOARD_SIZE = 800  # Taille fixe pour la board, même en fullscreen

# ✅ Centrage du plateau dans la fenêtre fullscreen
board_x = (SCREEN_WIDTH - BOARD_SIZE) // 2
board_y = (SCREEN_HEIGHT - BOARD_SIZE) // 2
rect = pygame.Rect(board_x, board_y, BOARD_SIZE, BOARD_SIZE)

# ------------------------------
# Initialisation du jeu
# ------------------------------
chess_game = cm.Game()

# ✅ Création de l'échiquier
chess_board = ChessBoard(rect, BOARD_SIZE, chess_game)

# ✅ Chargement des images (taille d'une case en pixels)
img.IMAGES = img.ImageLoader(BOARD_SIZE // 8)

# ------------------------------
# SceneManager principal
# ------------------------------
scene_manager = SceneManager(fenetre, SceneId.MAINMENU)

# ------------------------------
# Boucle principale du jeu
# ------------------------------
running = True
while running:
    running = scene_manager.update_loop()

pygame.quit()
