from .ui_element import UIElement
import pygame

class Slider(UIElement):
    def __init__(self, rect, min_value, max_value, initial_value):
        super().__init__(rect)  
        self.min_value = min_value
        self.max_value = max_value
        self.value = initial_value

        # Size and position of the slider handle
        self.slider_width = 20
        self.slider_rect = pygame.Rect(0, 0, self.slider_width, self.dest_rect.height + 10)
        self.slider_rect.centery = self.dest_rect.centery
        self._update_slider_position()  # Initial position

        self.dragging = False

    def _update_slider_position(self):
        """Updates the position of the slider handle based on the current value."""
        percent = (self.value - self.min_value) / (self.max_value - self.min_value)
        self.slider_rect.x = self.dest_rect.x + percent * (self.dest_rect.width - self.slider_width)

    def on_click(self, point: tuple[int, int]) -> bool:
        """Triggers handle movement when clicked."""
        if self.slider_rect.collidepoint(point):
            self.dragging = True
            return True  
        return False

    def on_hover(self, point: tuple[int, int]) -> bool:
        """Handles handle movement when the mouse is held down."""
        if self.dragging:
            # Limit handle movement within the slider bar
            new_x = min(max(point[0] - self.slider_width // 2, self.dest_rect.x), self.dest_rect.right - self.slider_width)
            self.slider_rect.x = new_x

            # Convert handle position into an integer value
            relative_x = self.slider_rect.x - self.dest_rect.x
            percent = relative_x / (self.dest_rect.width - self.slider_width)
            self.value = round(self.min_value + percent * (self.max_value - self.min_value))

            # Recalibrate position to align with integer values
            self._update_slider_position()
            return True  
        return False

    def on_mouseleave(self, point: tuple[int, int]) -> bool:
        """Stops handle movement when the mouse is released."""
        self.dragging = False
        return False

    def draw(self, surface):
        """Draws the slider."""
        pygame.draw.rect(surface, (200, 200, 200), self.dest_rect, 2)  #Slider bar
        pygame.draw.rect(surface, (0, 0, 0), self.slider_rect) 

        font = pygame.font.Font(None, 36)
        value_text = font.render(str(self.value), True, (0, 0, 0))
        surface.blit(value_text, (self.slider_rect.centerx - value_text.get_width() // 2, self.dest_rect.y - 30))

    def get_value(self):
        """Returns the current slider value."""
        return self.value
    
    def update(self) -> None:
        pass  



