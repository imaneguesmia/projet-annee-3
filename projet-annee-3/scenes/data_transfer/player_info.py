from .player_type import PlayerType

import cpp_chess as cm

from dataclasses import dataclass

@dataclass
class PlayerInfo:
    type: PlayerType
    settings: cm.EvaluatorSettings | None