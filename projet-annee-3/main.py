import cpp_chess as cm
import pygame
from scenes import SceneManager, SceneId
import image_loader as img

# ------------------------------
# Initialisation de Pygame
# ------------------------------
pygame.init()

# ✅ Fenêtre en FULLSCREEN sur l'écran actif
fenetre = pygame.display.set_mode(flags=pygame.FULLSCREEN)
SCREEN_WIDTH, SCREEN_HEIGHT = fenetre.get_size()

pygame.display.set_caption("Beluga Engine")

# ------------------------------
# Constantes de l'échiquier
# ------------------------------
BOARD_SIZE = 800  # Taille fixe pour la board, même en fullscreen

# ------------------------------
# Initialisation du jeu
# ------------------------------
# chess_game = cm.Game()

# ✅ Création de l'échiquier
# chess_board = ChessBoard(rect, BOARD_SIZE, chess_game)

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
