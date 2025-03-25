import cpp_chess as cm

from abc import ABC, abstractmethod

class PromotionCallbackInterface(ABC):
    """Interface for pawn promotion handling."""

    @abstractmethod
    def prompt_promotion(self, move: cm.Move) -> None: ...

    @abstractmethod
    def do_current_promotion(self, to_piece: cm.PType) -> None: ...