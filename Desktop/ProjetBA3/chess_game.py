import chess

class ChessGame:
    def __init__(self):
        self.plateau = chess.Board()
        self.piece_selectionnee = None
        self.coups_possibles = set()

    def selectionner_piece(self, case):
        if self.plateau.piece_at(case):
            self.piece_selectionnee = case
            # Récupère les cases vers lesquelles la pièce peut se déplacer
            self.coups_possibles = {coup.to_square for coup in self.plateau.legal_moves if coup.from_square == case}

    def jouer_coup(self, case):
        if self.piece_selectionnee:
            coup = chess.Move(self.piece_selectionnee, case)
            if coup in self.plateau.legal_moves:
                self.plateau.push(coup)
            self.piece_selectionnee = None
            self.coups_possibles.clear()
