from enum import Enum, auto

from dataclasses import dataclass

class SceneId(Enum):
    QUIT            = auto()

    TEST            = auto()
    MAINMENU        = auto()
    GAME            = auto()
    DIFFICULTY      = auto()
    CUSTOM          = auto()

@dataclass
class SceneChange:
    scene_id: SceneId
    kwargs: dict[str]