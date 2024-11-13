import chess

class ChessGame:
    def __init__(self):
        self.board = chess.Board()
        self.selected_piece = None
        self.possible_moves = set()

    def select_piece(self, square):
        if self.board.piece_at(square):
            self.selected_piece = square
            # Retrieve the squares to which the piece can move
            self.possible_moves = {move.to_square for move in self.board.legal_moves if move.from_square == square}

    def make_move(self, square):
        if self.selected_piece:
            move = chess.Move(self.selected_piece, square)
            if move in self.board.legal_moves:
                self.board.push(move)
            self.selected_piece = None
            self.possible_moves.clear()