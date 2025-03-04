from image_loader import ImageLoader

import cpp_chess as cm

import pygame

class ChessBoard:
    def __init__(self, window: pygame.Surface, board_size: int, chess_game: cm.Game):
        self.window = window
        self.square_size = board_size // 8

        self.chess_game = chess_game
        # self.legal_moves = self.chess_game.get_current_pseudo_legals()
        
        # Chargement des images des pièces
        self.images = ImageLoader(self.square_size)

        self.selected_square = None
        self.selected_moves = []  # Stocker les coups légaux de la pièce sélectionnée

    def draw_board(self):
        """Dessine le plateau avec les pièces et les coups légaux."""
        for row in range(8):
            for col in range(8):
                square = cm.Position(row, col)
                piece_on_square = self.chess_game.get_piece_at(square)
                coords = self.square_to_coordinates(square)

                # Déterminer la couleur de la case
                tile_index = (row + col) % 2

                tile = self.images.tile_sprite(tile_index)
                self.window.blit(tile, coords)

                if self.selected_square is not None:
                    # Dessiner la case selectionnée
                    if self.selected_square == square:
                        self.window.blit(self.images.selected_sprite(tile_index), coords)
                    # Dessiner les coups légaux 
                    else:
                        possible_move = next((move for move in self.selected_moves if cm.Position(move.target) == square), None)

                        if possible_move is not None:
                            self.window.blit(self.images.attacked_sprite(tile_index, possible_move.capture), coords)

                # Dessiner la pièce sur la case
                if piece_on_square.fen() != ".":
                    piece_image = self.images.piece_sprite(piece_on_square)
                    self.window.blit(piece_image, coords)

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
            self.selected_moves = self.get_legal_moves_from_square(square)

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

    def get_legal_moves_from_square(self, square: cm.Position) -> list[cm.Move]:
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