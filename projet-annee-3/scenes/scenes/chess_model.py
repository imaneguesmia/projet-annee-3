import cpp_chess as cm

from abc import ABC, abstractmethod

class ChessModel(ABC):
    """Interface for the "controller" (UI elements) to access the "model" (the scene) handling game state."""

    @abstractmethod
    def piece_at(self, square: cm.Position) -> cm.Piece: ...

    @abstractmethod
    def prompt_promotion(self, move: cm.Move) -> None: ...