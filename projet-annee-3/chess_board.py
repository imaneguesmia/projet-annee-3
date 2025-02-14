import cpp_chess as cm
import pygame

class ChessBoard:
    light_color = (232, 235, 239)
    dark_color = (125, 135, 150)
    selection_color = (144, 238, 144)
    move_color = (255, 255, 153)
    transparency = 128 

    def __init__(self, window: pygame.Surface, board_size: int, chess_game: cm.Game):
        self.window = window
        self.square_size = board_size // 8

        self.chess_game = chess_game
        self.legal_moves = self.chess_game.get_current_pseudo_legals()

        self.move_surface = pygame.Surface((self.square_size, self.square_size))
        self.move_surface.set_alpha(self.transparency)
        self.move_surface.fill(self.move_color)
        
        # Chargement des images des pièces
        self.piece_images = self.load_piece_images()

        self.selected_square = None
        self.selected_moves = []  # Stocker les coups légaux de la pièce sélectionnée

    def load_piece_images(self):
        """Charge les images des pièces."""
        pieces = {
            "P": pygame.image.load("projet-annee-3/images/white_pawn.png"),
            "p": pygame.image.load("projet-annee-3/images/black_pawn.png"),
            "R": pygame.image.load("projet-annee-3/images/white_rook.png"),
            "r": pygame.image.load("projet-annee-3/images/black_rook.png"),
            "N": pygame.image.load("projet-annee-3/images/white_knight.png"),
            "n": pygame.image.load("projet-annee-3/images/black_knight.png"),
            "B": pygame.image.load("projet-annee-3/images/white_bishop.png"),
            "b": pygame.image.load("projet-annee-3/images/black_bishop.png"),
            "Q": pygame.image.load("projet-annee-3/images/white_queen.png"),
            "q": pygame.image.load("projet-annee-3/images/black_queen.png"),
            "K": pygame.image.load("projet-annee-3/images/white_king.png"),
            "k": pygame.image.load("projet-annee-3/images/black_king.png"),
        }
        for key in pieces:
            pieces[key] = pygame.transform.scale(pieces[key], (self.square_size, self.square_size))
        return pieces

    def draw_board(self):
        """Dessine le plateau avec les pièces et les coups légaux."""
        for row in range(8):
            for col in range(8):
                square = cm.Position(row, col)
                piece_on_square = self.chess_game.get_piece_at(square)
                x, y = self.square_to_coordinates(square)

                # Déterminer la couleur de la case
                color = self.light_color if (row + col) % 2 == 0 else self.dark_color
                if self.selected_square is not None and self.selected_square == square:

                    color = self.selection_color  # Colorer la case sélectionnée

                pygame.draw.rect(self.window, color, pygame.Rect(x, y, self.square_size, self.square_size))

                # Dessiner les coups légaux 
                if self.selected_square and any(cm.Position(move.target) == square for move in self.selected_moves):
                    self.window.blit(self.move_surface, (x, y))

                # Dessiner la pièce sur la case
                if piece_on_square.fen() != ".":
                    self.window.blit(
                        self.piece_images[piece_on_square.fen()],
                        (x, y)
                    )

    def handle_click(self, x, y):
        """Gère les interactions quand un joueur clique sur une case."""
        square = self.coordinates_to_square(x, y)
        piece_on_square = self.chess_game.get_piece_at(square)
        current_player = self.chess_game.current_player

        # Mise à jour des coups légaux
        self.legal_moves = self.chess_game.get_current_legals()

        # Sélectionner une pièce si elle appartient au joueur courant
        if not piece_on_square.is_none() and piece_on_square.get_player() == current_player:
            self.selected_square = square
            self.selected_moves = self.get_legal_moves(square)

            print(f"Pièce sélectionnée : {piece_on_square.get_type()} ({piece_on_square.get_player()})") #  debug selection

        # Jouer un coup si une pièce est déjà sélectionnée et qu'on clique sur une case valide
        elif self.selected_square and any(cm.Position(move.target) == square for move in self.selected_moves):
            move_to_do = next((move for move in self.selected_moves if cm.Position(move.target) == square), None)
            if move_to_do:
                self.chess_game.move(move_to_do)
                print("Moved the piece")

                # Mettre à jour le plateau après un déplacement
                self.selected_square = None
                self.selected_moves = []
                self.legal_moves = self.chess_game.get_current_legals()

        else:
            self.selected_square = None
            self.selected_moves = []

    def get_legal_moves(self, square: cm.Position) -> list[cm.Move]:
        filtered_moves = []
        for move in self.legal_moves:
            move_position = cm.Position(move.source)

            if move_position == square:
                filtered_moves.append(move)

        print(filtered_moves)

        return filtered_moves


    def coordinates_to_square(self, x: int, y: int) -> cm.Position:
        row = y // self.square_size
        col = x // self.square_size
        return cm.Position(row, col)

    def square_to_coordinates(self, square: cm.Position) -> tuple[int, int]:
        x = self.square_size * square.column
        y = self.square_size * square.row
        return x, y