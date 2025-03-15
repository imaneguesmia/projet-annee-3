import cpp_chess as cm

from abc import ABC, abstractmethod

class ChessBoardCallbackInterface(ABC):
    """Interface for the "controller" (UI elements) to access the "model" (the scene) handling game state."""

    @abstractmethod
    def piece_at(self, square: cm.Position) -> cm.Piece: ...

    @abstractmethod
    def select_square(self, square: cm.Position) -> None: ...

    @abstractmethod
    def prompt_promotion(self, move: cm.Move) -> None: ...

    @abstractmethod
    def do_current_promotion(self, to_piece: cm.PType) -> None: ...

    @property
    @abstractmethod
    def selected_square(self) -> list[cm.Move]: ...

    @property
    @abstractmethod
    def selected_moves(self) -> list[cm.Move]: ...

    @abstractmethod
    def reshow_game_end_panel(self) -> None: ...