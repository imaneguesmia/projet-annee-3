import cpp_chess as cm

import pygame

from chess_board import ChessBoard

# Initialisation de Pygame
pygame.init()
window_size = 800
fenetre = pygame.display.set_mode((window_size, window_size))

# Initialisation des objets ChessGame et ChessBoard
chess_game = cm.Game()
chess_board = ChessBoard(fenetre, window_size, chess_game)


# Boucle principale
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            #print("clicked on screen")
            x, y = pygame.mouse.get_pos()
            chess_board.handle_click(x, y)  # Gère le clic de souris

    chess_board.draw_board()  # Dessine le plateau
    pygame.display.flip()

pygame.quit()

