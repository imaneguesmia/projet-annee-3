import cpp_chess as cm
import pygame
from chess_board import ChessBoard

# Initialisation de Pygame
pygame.init()
window_size = 800
fenetre = pygame.display.set_mode((window_size, window_size))
pygame.display.set_caption("Aqua Chess")

# Couleurs
AQUA_BLUE = (173, 216, 230)  
SAND_COLOR = (237, 201, 175)  
SAND_HOVER = (255, 228, 196) 
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Polices
title_font = pygame.font.Font(None, 80)
button_font = pygame.font.Font(None, 60)

# Boutons "Play" et "Quit"
button_play = pygame.Rect(window_size // 2 - 100, window_size // 2 - 40, 200, 80)
button_quit = pygame.Rect(window_size // 2 - 100, window_size // 2 + 60, 200, 80)

# Variable d'état du menu
menu_active = True

# Initialisation des objets ChessGame et ChessBoard
chess_game = cm.Game()
chess_board = ChessBoard(fenetre, window_size, chess_game)

def draw_menu():
    """Affiche le menu principal avec un titre et des boutons."""
    fenetre.fill(AQUA_BLUE)

    # Affichage du titre "Aqua Chess"
    title_surface = title_font.render("Aqua Chess", True, BLACK)
    title_rect = title_surface.get_rect(center=(window_size // 2, 150))
    fenetre.blit(title_surface, title_rect)

    # Récupérer position souris
    mouse_x, mouse_y = pygame.mouse.get_pos()

    # Dessiner les boutons Play et Quit
    for button, text in [(button_play, "Play"), (button_quit, "Quit")]:
        color = SAND_HOVER if button.collidepoint(mouse_x, mouse_y) else SAND_COLOR
        pygame.draw.rect(fenetre, color, button)

        # Texte du bouton
        text_surface = button_font.render(text, True, BLACK)
        text_rect = text_surface.get_rect(center=button.center)
        fenetre.blit(text_surface, text_rect)

    pygame.display.flip()


# Boucle principale
running = True
while running:
    if menu_active:
        draw_menu()
    else:
        chess_board.draw_board()
        pygame.display.flip()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        if menu_active:
            if event.type == pygame.MOUSEBUTTONDOWN:
                if button_play.collidepoint(event.pos):  
                    menu_active = False  
                elif button_quit.collidepoint(event.pos):  
                    running = False  
        else:
            if event.type == pygame.MOUSEBUTTONDOWN:
                x, y = pygame.mouse.get_pos()
                chess_board.handle_click(x, y)

pygame.quit()



