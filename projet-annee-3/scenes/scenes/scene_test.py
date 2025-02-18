from ..scene import Scene
from ..scene_change import SceneId

from ui import Text, Button

import pygame

class scene_Test(Scene):
    def __init__(self, window_rect: pygame.Rect):
        super().__init__(window_rect)

        self.bg_color = (200, 100, 200)

        # text = Text("hello world", window_rect.center)
        # text.color = (255, 0, 255)
        # text.bg_color = (255, 255, 0, 128)

        # child_text = Text("goodbye world", (0, 0))
        # child_text.color = (255, 255, 255)

        # text.add_child(child_text)

        # self.elements.append(text)

        button = Button(pygame.Rect(400, 400, 200, 100), "test scene")
        button.color = (0, 200, 0)
        button.text_color = (255, 0, 255)

        def oc(point: tuple[int, int]):
            self.request_scene_change(SceneId.MAINMENU, {})

            return True

        button.on_click = oc

        # child = Button(pygame.Rect(0, 0, 100, 50), "child")

        # def occ(self):
        #     print("Child clicked")

        #     return True
        
        # child.on_click = occ

        # button.add_child(child)

        self.elements.append(button)