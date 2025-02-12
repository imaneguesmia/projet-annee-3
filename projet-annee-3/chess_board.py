import cpp_chess as cm

import pygame

from chess_game import ChessGame

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
        self.legal_moves = self.chess_game.get_current_legals()

        self.move_surface = pygame.Surface((self.square_size, self.square_size))
        self.move_surface.set_alpha(self.transparency)
        self.move_surface.fill(self.move_color)
        
        # Load piece images
        self.piece_images = self.load_piece_images()

        self.selected_square = cm.Position()

    def load_piece_images(self):
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
        for row in range(8):
            for col in range(8):
                square = cm.Position(row, col)
                piece_on_square = self.chess_game.get_piece_at(square)

                # Get screen position of square
                x, y = self.square_to_coordinates(square)
                
                # Determine square color
                color = self.light_color if (row + col) % 2 == 0 else self.dark_color
                
                pygame.draw.rect(self.window, color, pygame.Rect(x, y, self.square_size, self.square_size))

                # Draw piece image if it exists
                if piece_on_square.fen() != ".":
                    self.window.blit(
                        self.piece_images[piece_on_square.fen()],
                        (x, y)
                    )
                
                # # Draw translucent surface for possible moves
                # if square in self.chess_game.possible_moves:
                #     self.window.blit(self.move_surface, (col * self.square_size, row * self.square_size))
                
                # # Draw piece if it exists
                # piece = self.chess_game.board.piece_at(square)
                # if piece:
                #     self.window.blit(self.piece_images[piece.symbol()], (col * self.square_size, row * self.square_size))

    def handle_click(self, x, y):
        square = self.coordinates_to_square(x, y)

        print(square, self.selected_square.is_valid)

        square_is_empty             = self.chess_game.get_piece_at(square).is_none()
        square_contains_friendly    = self.chess_game.get_piece_at(square).get_player() == self.chess_game.current_player
        piece_currently_selected    = self.selected_square.is_valid

        if not square_is_empty and square_contains_friendly:
            # Select piece
            print("Selected new square")
            self.selected_square = square
            print(
                self.chess_game.get_piece_at(self.selected_square).get_player(),
                self.chess_game.get_piece_at(self.selected_square).get_type()
            )
            self.draw_legal_moves()
        elif piece_currently_selected:
            # move_to_do = any(move.target == square for move in self.legal_moves_from_square(self.selected_square))
            move_to_do = next((move for move in self.legal_moves_from_square(self.selected_square) if move.target == square), None)

            if move_to_do is not None:
                self.chess_game.move(move_to_do)
                

        if not piece_currently_selected:
            if square_contains_friendly:
                # Select piece
                print("Selected new square")
                self.selected_square = square
                print(
                    self.chess_game.get_piece_at(self.selected_square).get_player(),
                    self.chess_game.get_piece_at(self.selected_square).get_type()
                )
                self.draw_legal_moves()
        
        else:


        if not self.chess_game.get_piece_at(square).is_none() and \
           self.chess_game.get_piece_at(square).get_player() == self.chess_game.current_player:
            # Select piece
            print("Selected new square")
            self.selected_square = square
            print(
                self.chess_game.get_piece_at(self.selected_square).get_player(),
                self.chess_game.get_piece_at(self.selected_square).get_type()
            )
            self.draw_legal_moves()
        elif 


        if self.chess_game.get_piece_at(square).is_none():
            # Clear selected piece
            print("Clicked empty")
            self.selected_square.set_position_invalid()
        elif self.chess_game.get_piece_at(square).get_player() == self.chess_game.current_player:
            # Select piece
            print("Selected new square")
            self.selected_square = square
            print(
                self.chess_game.get_piece_at(self.selected_square).get_player(),
                self.chess_game.get_piece_at(self.selected_square).get_type()
            )
            self.draw_legal_moves()
        elif self.selected_square.is_valid:
            # Make the move if a piece is selected
            # self.chess_game.make_move(square)
            pass

    def legal_moves_from_square(self, square: cm.Position) -> list[cm.Move]:
        return filter(lambda m: m.source == square, self.legal_moves)

    def draw_legal_moves(self) -> None:
        """"""
        moves_from_square = self.legal_moves_from_square(self.selected_square)

        for move in moves_from_square:
            self.window.blit(self.move_surface, self.square_to_coordinates(move.target))

    def coordinates_to_square(self, x: int, y: int) -> cm.Position:
        row = y // self.square_size
        col = x // self.square_size

        return cm.Position(row, col)

    def square_to_coordinates(self, square: cm.Position) -> tuple[int, int]:
        x = self.square_size * square.column
        y = self.square_size * square.row

        return x, y