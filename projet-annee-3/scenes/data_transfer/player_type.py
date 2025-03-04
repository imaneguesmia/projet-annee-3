from enum import Enum, auto

class PlayerType(Enum):
    HUMAN       = auto()
    MINIMAX     = auto()
    NEURAL_NET  = auto()