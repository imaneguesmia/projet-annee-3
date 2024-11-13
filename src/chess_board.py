import pygame
import chess
from chess_game import ChessGame

class ChessBoard:
    light_color = (232, 235, 239)
    dark_color = (125, 135, 150)
    selection_color = (144, 238, 144)
    move_color = (255, 255, 153)
    transparency = 128 

    def __init__(self, window, board_size, chess_game):
        self.window = window
        self.square_size = board_size // 8
        self.chess_game = chess_game
        self.move_surface = pygame.Surface((self.square_size, self.square_size))
        self.move_surface.set_alpha(self.transparency)
        self.move_surface.fill(self.move_color)
        
        # Load piece images
        self.pieces = self.load_piece_images()

    def load_piece_images(self):
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
            pieces[key] = pygame.transform.scale(pieces[key], (self.square_size, self.square_size))
        return pieces

    def draw_board(self):
        for row in range(8):
            for col in range(8):
                # Convert square for python-chess
                square = chess.square(col, 7 - row)
                
                # Determine square color
                if self.chess_game.selected_piece == square:
                    color = self.selection_color
                else:
                    color = self.light_color if (row + col) % 2 == 0 else self.dark_color
                
                pygame.draw.rect(self.window, color, pygame.Rect(col * self.square_size, row * self.square_size, self.square_size, self.square_size))
                
                # Draw translucent surface for possible moves
                if square in self.chess_game.possible_moves:
                    self.window.blit(self.move_surface, (col * self.square_size, row * self.square_size))
                
                # Draw piece if it exists
                piece = self.chess_game.board.piece_at(square)
                if piece:
                    self.window.blit(self.pieces[piece.symbol()], (col * self.square_size, row * self.square_size))

    def handle_click(self, x, y):
        square = self.coordinates_to_square(x, y)
        if self.chess_game.selected_piece:
            # Make the move if a piece is selected
            self.chess_game.make_move(square)
        else:
            # Select the piece
            self.chess_game.select_piece(square)

    def coordinates_to_square(self, x, y):
        row = 7 - (y // self.square_size)
        col = x // self.square_size
        return chess.square(col, row)