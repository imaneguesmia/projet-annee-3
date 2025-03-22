from __future__ import annotations
import json
from datetime import datetime
from typing import Dict, Any


class TrainLog:
    def __init__(self, train_id: str):
        self.train_id = train_id
        self.losses: list[tuple[int, float]] = []  # (epoch, loss)
        self.training_params: Dict[str, Any] = {}
        
        # Charger les logs existants si le fichier existe
        try:
            with open(f"runs/{train_id}.txt", "r") as f:
                in_params = False
                for line in f:
                    line = line.strip()
                    if line.startswith("Training Parameters"):
                        in_params = True
                        continue
                    elif line.startswith("Training Log"):
                        in_params = False
                        continue
                    elif not line:
                        continue
                        
                    if in_params:
                        if ": " in line:
                            key, value = line.split(": ", 1)
                            self.training_params[key] = value
                    else:
                        if line.startswith("epoch"):
                            _, data = line.split(": ", 1)
                            epoch_str, loss_str = data.split(" - loss: ")
                            self.losses.append((int(epoch_str), float(loss_str)))
        except FileNotFoundError:
            pass

    def set_training_params(self, params: Dict[str, Any]) -> None:
        """Enregistre les paramètres d'entraînement"""
        self.training_params = params
        self._save_header()

    def update(self, loss: float, current_epoch: int) -> None:
        """Ajoute une nouvelle valeur de loss avec son vrai numéro d'epoch"""
        self.losses.append((current_epoch, loss))
        self._append_log(current_epoch, loss)

    def _save_header(self) -> None:
        """Écrit l'en-tête avec les paramètres d'entraînement"""
        with open(f"runs/{self.train_id}.txt", "w") as log:
            log.write(f"Training Parameters (Started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')})\n")
            log.write("-" * 80 + "\n")
            for key, value in self.training_params.items():
                log.write(f"{key}: {value}\n")
            log.write("\nTraining Log\n")
            log.write("-" * 80 + "\n")

    def _append_log(self, epoch: int, loss: float) -> None:
        """Ajoute une nouvelle ligne de log"""
        with open(f"runs/{self.train_id}.txt", "a") as log:
            log.write(f"epoch: {epoch} - loss: {loss}\n")

    def save(self) -> None:
        logs = ""
        for epoch, loss in enumerate(self.losses):
            logs += f"epoch {epoch}: {loss}\n"
        with open(f"runs/{self.train_id}.txt", "w") as log:
            log.write(logs)
