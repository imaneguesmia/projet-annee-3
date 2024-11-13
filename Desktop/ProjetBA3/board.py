import pygame
import chess

# Initialisation de Pygame et du plateau d'échecs
pygame.init()
taille_fenetre = 800
taille_case = taille_fenetre // 8
fenetre = pygame.display.set_mode((taille_fenetre, taille_fenetre))
plateau = chess.Board()

# Couleurs des cases
couleur_claire = (232, 235, 239)
couleur_foncee = (125, 135, 150)
couleur_selection = (144, 238, 144)  # Vert clair pour la case sélectionnée
couleur_deplacement = (255, 255, 153)  # Jaune pâle
transparence = 128  # Niveau de transparence (0 à 255)
surface_deplacement = pygame.Surface((taille_case, taille_case))
surface_deplacement.set_alpha(transparence)
surface_deplacement.fill(couleur_deplacement)

# Chargement des images des pièces
pieces = {
    "P": pygame.image.load("images/white_pawn.png"),
    "p": pygame.image.load("images/black_pawn.png"),
    "R": pygame.image.load("images/white_rook.png"),
    "r": pygame.image.load("images/black_rook.png"),
    "N": pygame.image.load("images/white_knight.png"),
    "n": pygame.image.load("images/black_knight.png"),
    "B": pygame.image.load("images/white_bishop.png"),
    "b": pygame.image.load("images/black_bishop.png"),
    "Q": pygame.image.load("images/white_queen.png"),
    "q": pygame.image.load("images/black_queen.png"),
    "K": pygame.image.load("images/white_king.png"),
    "k": pygame.image.load("images/black_king.png"),
}

# Redimensionner les images pour qu'elles rentrent dans une case
for key in pieces:
    pieces[key] = pygame.transform.scale(pieces[key], (taille_case, taille_case))

# Fonction pour dessiner le plateau et les pièces
def dessiner_plateau(plateau, coups_possibles):
    for ligne in range(8):
        for colonne in range(8):
            # Conversion des coordonnées de Pygame en case chess
            case = chess.square(colonne, 7 - ligne)
            
            # Déterminer la couleur de la case
            if piece_selectionnee == case:
                couleur = couleur_selection  # Case de la pièce sélectionnée en vert
            else:
                couleur = couleur_claire if (ligne + colonne) % 2 == 0 else couleur_foncee
            
            # Dessiner la case de base
            pygame.draw.rect(fenetre, couleur, pygame.Rect(colonne * taille_case, ligne * taille_case, taille_case, taille_case))
            
            # Si la case est un déplacement possible, dessiner la surface translucide
            if case in coups_possibles:
                fenetre.blit(surface_deplacement, (colonne * taille_case, ligne * taille_case))

            # Dessiner la pièce si elle existe à cette position
            piece = plateau.piece_at(case)
            if piece:
                fenetre.blit(pieces[piece.symbol()], (colonne * taille_case, ligne * taille_case))


# Fonction pour convertir les clics en notation
def coordonnees_vers_case(x, y):
    ligne = 7 - (y // taille_case)  # Inverser l'axe des lignes pour Pygame
    colonne = x // taille_case
    return chess.square(colonne, ligne)

# Boucle principale
running = True
piece_selectionnee = None
coups_possibles = set()  # Initialiser un ensemble pour stocker les cases de déplacement possibles

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            x, y = pygame.mouse.get_pos()
            case = coordonnees_vers_case(x, y)

            if piece_selectionnee:
                # Si une pièce est sélectionnée, tente de faire le coup
                coup = chess.Move(piece_selectionnee, case)
                if coup in plateau.legal_moves:
                    plateau.push(coup)
                piece_selectionnee = None
                coups_possibles.clear()  # Réinitialise les coups possibles après le déplacement
            else:
                # Sélectionner une pièce si elle est à cet endroit
                if plateau.piece_at(case):
                    piece_selectionnee = case
                    # Obtenir tous les coups légaux pour cette pièce
                    coups_possibles = {coup.to_square for coup in plateau.legal_moves if coup.from_square == case}

    dessiner_plateau(plateau, coups_possibles)
    pygame.display.flip()

pygame.quit()



