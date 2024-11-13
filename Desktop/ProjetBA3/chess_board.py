import pygame
import chess
from chess_game import ChessGame

class ChessBoard:
    couleur_claire = (232, 235, 239)
    couleur_foncee = (125, 135, 150)
    couleur_selection = (144, 238, 144)
    couleur_deplacement = (255, 255, 153)
    transparence = 128  # Niveau de transparence

    def __init__(self, fenetre, taille_fenetre, jeu_echecs):
        self.fenetre = fenetre
        self.taille_case = taille_fenetre // 8
        self.jeu_echecs = jeu_echecs
        self.surface_deplacement = pygame.Surface((self.taille_case, self.taille_case))
        self.surface_deplacement.set_alpha(self.transparence)
        self.surface_deplacement.fill(self.couleur_deplacement)
        
        # Chargement des images des pièces
        self.pieces = self.charger_images_pieces()

    def charger_images_pieces(self):
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
        for key in pieces:
            pieces[key] = pygame.transform.scale(pieces[key], (self.taille_case, self.taille_case))
        return pieces

    def dessiner_plateau(self):
        for ligne in range(8):
            for colonne in range(8):
                # Conversion de la case pour python-chess
                case = chess.square(colonne, 7 - ligne)
                
                # Déterminer la couleur de la case
                if self.jeu_echecs.piece_selectionnee == case:
                    couleur = self.couleur_selection
                else:
                    couleur = self.couleur_claire if (ligne + colonne) % 2 == 0 else self.couleur_foncee
                
                pygame.draw.rect(self.fenetre, couleur, pygame.Rect(colonne * self.taille_case, ligne * self.taille_case, self.taille_case, self.taille_case))
                
                # Dessiner la surface translucide pour les coups possibles
                if case in self.jeu_echecs.coups_possibles:
                    self.fenetre.blit(self.surface_deplacement, (colonne * self.taille_case, ligne * self.taille_case))
                
                # Dessiner la pièce si elle existe
                piece = self.jeu_echecs.plateau.piece_at(case)
                if piece:
                    self.fenetre.blit(self.pieces[piece.symbol()], (colonne * self.taille_case, ligne * self.taille_case))

    def handle_click(self, x, y):
        case = self.coordonnees_vers_case(x, y)
        if self.jeu_echecs.piece_selectionnee:
            # Jouer le coup si une pièce est sélectionnée
            self.jeu_echecs.jouer_coup(case)
        else:
            # Sélectionner la pièce
            self.jeu_echecs.selectionner_piece(case)

    def coordonnees_vers_case(self, x, y):
        ligne = 7 - (y // self.taille_case)
        colonne = x // self.taille_case
        return chess.square(colonne, ligne)
