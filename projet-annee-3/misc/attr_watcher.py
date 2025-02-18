class AttributeWatcher:
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.__changed: dict[str, bool] = {}

    def __setattr__(self, name: str, value) -> None:
        try:
            changed = getattr(self, name) != value
        except AttributeError:
            changed = False

        super().__setattr__(name, value)

        if name in self.__changed:
            self.__changed[name] = self.__changed[name] or changed
    
    def is_changed(self, attr: str) -> bool:
        """Checks if the given attribute changed.

        Args:
            attr (str): The name of the attribute to check.

        Returns:
            bool: `True` if the attribute changed.
        """
        return self.__changed[attr]
    
    def all_changed(self) -> list[str]:
        """Returns a list of the names of all attributes that changed."""
        return [k for k in self.__changed if self.is_changed(k)]

    def reset_changed(self) -> None:
        """Resets all attribute changed flags."""
        for k in self.__changed:
            self.__changed[k] = False
    
    def peek_changed(self, attr: str) -> bool:
        return self.__changed[attr]
    
    def watch(self, *attrs: str) -> None:
        """Watch the given attributes on self."""
        for k in attrs:
            self.__changed[k] = False
    
    @property
    def watched_attrs(self) -> list[str]:
        """List of watched attributes."""
        return self.__changed.keys()