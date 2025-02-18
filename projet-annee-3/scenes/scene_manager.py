from .scene import Scene
from .scene_change import SceneId

from .scenes.scene_test import scene_Test
from .scenes.scene_main_menu import scene_MainMenu

import pygame

from typing import override

class SceneManager:
    @staticmethod
    def _id_to_scene_type(id: SceneId) -> type[Scene]:
        """"""
        match id:
            case SceneId.TEST: return scene_Test
            case SceneId.MAINMENU: return scene_MainMenu

    def __init__(self, window: pygame.Surface, initial_scene: SceneId, **kwargs: dict[str]):
        super().__init__()

        self._window = window

        self._current_scene: Scene = None

        self.switch_scene(initial_scene, **kwargs)
    
    @override
    def switch_scene(self, scene_id: SceneId, **kwargs: dict[str]) -> None:
        """"""
        scene_type = self._id_to_scene_type(scene_id)
        self._current_scene = scene_type(self._window.get_rect(), **kwargs)
    
    def update_loop(self) -> bool:
        """Handles the update loop. Returns `False` if the game is closed."""
        running  = True

        self._current_scene.update()
        self._current_scene.draw(self._window)

        pygame.display.flip()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            else:
                self._current_scene.handle_event(event)
        
        scene_change = self._current_scene.requested_scene_change()

        if running and scene_change is not None:
            if scene_change.scene_id == SceneId.QUIT:
                running = False
            else:
                self.switch_scene(scene_change.scene_id, **scene_change.kwargs)
        
        return running