from __future__ import annotations

import argparse
import json
import os
import pathlib
import sys
from typing import Optional

from dataloader import BatchLoader
from model import (
    NnBoard768Cuda,
    NnBoard768,
    NnHalfKA,
    NnHalfKACuda,
    NnHalfKP,
    NnHalfKPCuda,
)
from time import time

import torch
from trainlog import TrainLog

DEVICE = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

LOG_ITERS = 500_000  


class WeightClipper:
    def __init__(self, frequency=1):
        self.frequency = frequency

    def __call__(self, module):
        if hasattr(module, "weight"):
            w = module.weight.data
            w = w.clamp(-1.98, 1.98)
            module.weight.data = w


def find_latest_checkpoint(train_id: str) -> Optional[tuple[int, str]]:
    """Trouve le dernier checkpoint pour un train_id donné"""
    checkpoints = []
    for file in os.listdir("nn"):
        if file.startswith(f"{train_id}_") and not file.endswith(".json"):
            try:
                epoch = int(file.split("_")[-1])
                checkpoints.append((epoch, os.path.join("nn", file)))
            except ValueError:
                continue
    return max(checkpoints) if checkpoints else None


def train(
    model: torch.nn.Module,
    optimizer: torch.optim.Optimizer,
    dataloader: BatchLoader,
    wdl: float,
    scale: float,
    epochs: int,
    save_epochs: int,
    train_id: str,
    start_epoch: int = 0,
    lr_drop: int | None = None,
    train_log: TrainLog | None = None,
) -> None:
    clipper = WeightClipper()
    running_loss = torch.zeros((1,), device=DEVICE)
    start_time = time()
    iterations = 0

    loss_since_log = torch.zeros((1,), device=DEVICE)
    iter_since_log = 0

    fens = 0
    epoch = start_epoch

    while epoch < epochs:
        new_epoch, batch = dataloader.read_batch(DEVICE)
        
        if new_epoch:
            epoch += 1
            if epoch == lr_drop:
                optimizer.param_groups[0]["lr"] *= 0.1
            print(
                f"epoch {epoch}",
                f"epoch train loss: {running_loss.item() / iterations}",
                f"epoch pos/s: {fens / (time() - start_time)}",
                sep=os.linesep,
            )

            running_loss = torch.zeros((1,), device=DEVICE)
            start_time = time()
            iterations = 0
            fens = 0

            if epoch % save_epochs == 0:
                torch.save(model.state_dict(), f"nn/{train_id}_{epoch}")
                param_map = {
                    name: param.detach().cpu().numpy().tolist()
                    for name, param in model.named_parameters()
                }
                with open(f"nn/{train_id}.json", "w") as json_file:
                    json.dump(param_map, json_file)

        optimizer.zero_grad()
        prediction = model(batch)
        expected = torch.sigmoid(batch.cp / scale) * (1 - wdl) + batch.wdl * wdl

        loss = torch.mean((prediction - expected) ** 2)
        loss.backward()
        optimizer.step()
        model.apply(clipper)

        with torch.no_grad():
            running_loss += loss
            loss_since_log += loss
        iterations += 1
        iter_since_log += 1
        fens += batch.size

        if iter_since_log * batch.size > LOG_ITERS:
            loss = loss_since_log.item() / iter_since_log
            print(
                f"At {iterations * batch.size} positions",
                f"Running Loss: {loss}",
                sep=os.linesep,
            )
            if train_log is not None:
                train_log.update(loss, epoch)
            iter_since_log = 0
            loss_since_log = torch.zeros((1,), device=DEVICE)


def main():
    parser = argparse.ArgumentParser(description="")

    parser.add_argument(
        "--data-root", type=str, help="Root directory of the data files"
    )
    parser.add_argument("--train-id", type=str, help="ID to save train logs with")
    parser.add_argument("--lr", type=float, help="Initial learning rate")
    parser.add_argument("--epochs", type=int, help="Epochs to train for")
    parser.add_argument("--batch-size", type=int, default=16384, help="Batch size")
    parser.add_argument("--wdl", type=float, default=0.0, help="WDL weight to be used")
    parser.add_argument("--scale", type=float, help="WDL weight to be used")
    parser.add_argument(
        "--save-epochs",
        type=int,
        default=100,
        help="How often the program will save the network",
    )
    parser.add_argument(
        "--lr-drop",
        type=int,
        default=None,
        help="The epoch learning rate will be dropped",
    )
    parser.add_argument(
        "--continue-from",
        type=str,
        help="Continue training from this checkpoint (if not specified, will look for latest checkpoint of train-id)",
    )
    args = parser.parse_args()

    assert args.train_id is not None
    assert args.scale is not None

    print("Initialisation du TrainLog...")
    train_log = TrainLog(args.train_id)
    
    # Enregistrer les paramètres d'entraînement
    train_params = {
        "learning_rate": args.lr,
        "epochs": args.epochs,
        "batch_size": args.batch_size,
        "wdl_weight": args.wdl,
        "scale": args.scale,
        "save_epochs": args.save_epochs,
        "lr_drop": args.lr_drop,
        "device": str(DEVICE),
        "architecture": "NnHalfKP(128)",
    }
    train_log.set_training_params(train_params)

    print("Création du modèle...")
    model = NnHalfKP(128).to(DEVICE)
    start_epoch = 0

    # Charger un checkpoint existant si demandé
    if args.continue_from:
        checkpoint_path = args.continue_from
        if os.path.exists(checkpoint_path):
            print(f"Chargement du checkpoint : {checkpoint_path}")
            model.load_state_dict(torch.load(checkpoint_path))
            try:
                start_epoch = int(checkpoint_path.split("_")[-1])
            except ValueError:
                print("Impossible de déterminer l'epoch de départ du checkpoint")
    else:
        # Chercher le dernier checkpoint pour ce train_id
        latest = find_latest_checkpoint(args.train_id)
        if latest:
            start_epoch, checkpoint_path = latest
            print(f"Reprise de l'entraînement depuis le checkpoint : {checkpoint_path}")
            model.load_state_dict(torch.load(checkpoint_path))

    print(f"Modèle créé et déplacé sur {DEVICE}")

    data_path = pathlib.Path(args.data_root)
    paths = list(map(str, data_path.glob("*.bin")))
    print(f"Fichiers de données trouvés : {paths}")
    
    if not paths:
        print("ERREUR : Aucun fichier .bin trouvé dans le dossier data/")
        print("Vous devez d'abord convertir les fichiers texte en format binaire.")
        sys.exit(1)
    
    print("Initialisation du DataLoader...")
    dataloader = BatchLoader(paths, model.input_feature_set(), args.batch_size)
    print("DataLoader initialisé")

    print("Configuration de l'optimiseur...")
    optimizer = torch.optim.Adam(model.parameters(), lr=args.lr)
    print("Optimiseur configuré")

    print(f"Début de l'entraînement à partir de l'epoch {start_epoch}...")
    train(
        model,
        optimizer,
        dataloader,
        args.wdl,
        args.scale,
        args.epochs,
        args.save_epochs,
        args.train_id,
        start_epoch=start_epoch,
        lr_drop=args.lr_drop,
        train_log=train_log,
    )


if __name__ == "__main__":
    main()
