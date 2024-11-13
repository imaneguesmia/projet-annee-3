import pygame
from chess_game import ChessGame
from chess_board import ChessBoard

# Initialisation de Pygame
pygame.init()
taille_fenetre = 800
fenetre = pygame.display.set_mode((taille_fenetre, taille_fenetre))

# Initialisation des objets ChessGame et ChessBoard
jeu_echecs = ChessGame()
plateau_graphique = ChessBoard(fenetre, taille_fenetre, jeu_echecs)

# Boucle principale
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            x, y = pygame.mouse.get_pos()
            plateau_graphique.handle_click(x, y)  # Gère le clic de souris

    plateau_graphique.dessiner_plateau()  # Dessine le plateau
    pygame.display.flip()

pygame.quit()
