from .scene_change import SceneId

from abc import ABC, abstractmethod

class SceneChangeInterface(ABC):
    """Interface exposing scene change methods."""
    @abstractmethod
    def request_scene_change(self, scene_id: SceneId, kwargs: dict[str]) -> None: ...