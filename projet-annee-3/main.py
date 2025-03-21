from scenes import SceneManager, SceneId
import image_loader as img

import pygame

# ------------------------------
# Initialisation de Pygame
# ------------------------------
pygame.init()

# ✅ Fenêtre en FULLSCREEN sur l'écran actif
# fenetre = pygame.display.set_mode(flags=pygame.FULLSCREEN)
fenetre = pygame.display.set_mode((1440, 900))
SCREEN_WIDTH, SCREEN_HEIGHT = fenetre.get_size()

pygame.display.set_caption("Beluga Engine")

# ------------------------------
# Constantes de l'échiquier
# ------------------------------
BOARD_SIZE = 800  # Taille fixe pour la board, même en fullscreen

# ------------------------------
# Initialisation du jeu
# ------------------------------

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
