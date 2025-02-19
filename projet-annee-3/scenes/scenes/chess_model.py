import cpp_chess as cm

from abc import ABC, abstractmethod

class ChessModel(ABC):
    """Interface for the "controller" to access the "model" handling game state."""

    @abstractmethod
    def piece_at(self, square: cm.Position) -> cm.Piece: ...