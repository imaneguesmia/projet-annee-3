#!/usr/bin/env python3

# python nnue_server.py --model-path "nn/beluga_v1_10" --model-type "board768" --scale 400
import argparse
import socket
import sys
import torch
import math
import chess  # pip install chess (python-chess)
import threading
import queue

# ---------------------------------------------------------------------
# 0. Import de classes de réseau
# ---------------------------------------------------------------------
from model import (
    NnBoard768, NnHalfKA, NnHalfKP,
    NnBoard768Cuda, NnHalfKACuda, NnHalfKPCuda
)

# ---------------------------------------------------------------------
# 1. Fonctions utilitaires
# ---------------------------------------------------------------------
def color_to_int(is_white: bool) -> int:
    return 0 if is_white else 1

def piece_to_int(piece: chess.Piece) -> int:
    """
    chess.Piece.piece_type:
      Pawn=1, Knight=2, Bishop=3, Rook=4, Queen=5, King=6
    Nous voulons 0..5 => (type - 1).
    """
    return piece.piece_type - 1

def flip_rank(sq: int) -> int:
    """
    Inverse la rangée, comme le fait CozyChess en Rust: sq ^ 56.
    """
    return sq ^ 56

def feature(perspective: int, color: int, piece_type: int, square: int) -> int:
    """
    Reproduit le calcul d'index "Board768" (perspective).
    """
    if perspective == 1:  # Si c'est Black to move
        square = flip_rank(square)
        color = 1 - color
    index = 0
    index = index * 2 + color
    index = index * 6 + piece_type
    index = index * 64 + square
    return index

# ---------------------------------------------------------------------
# 2. Construction d'un Batch unique pour la FEN (Board768)
# ---------------------------------------------------------------------
from dataloader import Batch

def fen_to_input_tensor(fen: str, device: torch.device) -> Batch:
    """
    Construit un Batch unitaire (size=1) selon le schéma Board768
    (indices_per_feature=2).
    """
    board = chess.Board(fen)
    stm_color = color_to_int(board.turn)  # 0 = White, 1 = Black

    stm_indices_list = []
    nstm_indices_list = []
    values_list = []

    for sq in range(64):
        piece = board.piece_at(sq)
        if piece is None:
            continue
        c = color_to_int(piece.color)
        pt = piece_to_int(piece)
        stm_feat  = feature(stm_color, c, pt, sq)
        nstm_feat = feature(1 - stm_color, c, pt, sq)

        # Board768 => 2 indices par feature: (batch_idx, feature)
        stm_indices_list.append(0)
        stm_indices_list.append(stm_feat)
        nstm_indices_list.append(0)
        nstm_indices_list.append(nstm_feat)
        values_list.append(1.0)

    stm_indices_tensor  = torch.tensor(stm_indices_list,  dtype=torch.long, device=device)
    nstm_indices_tensor = torch.tensor(nstm_indices_list, dtype=torch.long, device=device)
    values_tensor       = torch.tensor(values_list,       dtype=torch.float32, device=device)

    # cp=0, wdl=0.5 => placeholders (non utilisés pour l'éval)
    cp_tensor  = torch.zeros((1,1), dtype=torch.float32, device=device)
    wdl_tensor = torch.full((1,1), 0.5, dtype=torch.float32, device=device)

    return Batch(
        stm_indices=stm_indices_tensor,
        nstm_indices=nstm_indices_tensor,
        values=values_tensor,
        cp=cp_tensor,
        wdl=wdl_tensor,
        size=1
    )

# ---------------------------------------------------------------------
# 3. Évaluation : p -> centipawns
# ---------------------------------------------------------------------
def sigmoid_to_centipawns(p: float, scale: float) -> float:
    """
    Reproduit : pred_cp = -torch.log(1.0 / p - 1.0) * scale
    en évitant log(0).
    """
    eps = 1e-9
    if p <= 0.0:   p = eps
    if p >= 1.0:   p = 1.0 - eps
    cp = -math.log((1.0 / p) - 1.0) * scale
    return cp


def evaluate_fen(model, fen, scale):
    """
    Fait la même chose que le script evaluate_model pour *une* FEN,
    en supposant qu'on veut la perspective 'side to move'.
    """
    # 1) Parser la FEN pour savoir s'il s'agit de White ou Black to move
    board = chess.Board(fen)
    side_to_move = board.turn  # True=White, False=Black

    # 2) Construire un batch "unitaire" en reprenant le code Board768
    #    Sauf qu'on va initialiser batch.cp selon le trait :
    #       cp = 0 si White to move
    #       cp = 0 si Black to move
    #    Mais, de façon plus réaliste, si vous aviez un 'cp' "objectif", vous le mettriez
    #    en positif pour White, en négatif pour Black, etc.
    #    Pour l'instant, on met cp=0 ; c'est ainsi que vous faites dans le script "server".
    batch = fen_to_input_tensor(fen, device=DEVICE)

    # 3) On appelle le réseau
    with torch.no_grad():
        prediction = model(batch)   # p = proba side to move
        p = float(prediction.item())

    # 4) Convertir p -> cp (toujours du point de vue 'side to move') via la même formule
    #    que "evaluate_model" :
    #       pred_cp = -log((1/p)-1)*scale
    if p <= 0.0:
        p = 1e-9
    if p >= 1.0:
        p = 1.0 - 1e-9
    pred_cp = -math.log((1.0 / p) - 1.0) * scale


    return pred_cp

# ---------------------------------------------------------------------
# 4. Classe pour gérer les connexions clients
# ---------------------------------------------------------------------
class ClientHandler(threading.Thread):
    def __init__(self, conn, addr, model, scale):
        """
        Initialise un thread pour gérer une connexion client.
        """
        threading.Thread.__init__(self)
        self.conn = conn
        self.addr = addr
        self.model = model
        self.scale = scale
        # Utilisation d'un mutex pour protéger l'accès au modèle
        self.model_lock = threading.Lock()
    
    def run(self):
        """
        Traite les requêtes du client.
        """
        print(f"[Serveur] Traitement de la connexion depuis {self.addr}")
        data_buffer = b""
        
        try:
            while True:
                chunk = self.conn.recv(1024)
                if not chunk:
                    # Le client a fermé la connexion
                    print(f"[Serveur] Connexion fermée par {self.addr}")
                    break
                
                data_buffer += chunk
                
                # Tant qu'on peut extraire une ligne
                while b"\n" in data_buffer:
                    line, data_buffer = data_buffer.split(b"\n", 1)
                    fen = line.decode("utf-8").strip()
                    if not fen:
                        continue  # ligne vide éventuelle
                    
                    # Évaluer la FEN avec le mutex pour éviter les conflits d'accès au modèle
                    with self.model_lock:
                        score_cp = evaluate_fen(self.model, fen, self.scale)
                    
                    msg = f"{score_cp:.2f}\n"
                    self.conn.sendall(msg.encode("utf-8"))
        except Exception as e:
            print(f"[Serveur] Erreur avec client {self.addr}: {e}")
        finally:
            self.conn.close()
            print(f"[Serveur] Connexion avec {self.addr} fermée")

# ---------------------------------------------------------------------
# 5. MAIN: Lancement d'un serveur socket, arguments => model, scale, etc.
# ---------------------------------------------------------------------
DEVICE = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--model-path", required=True, help="Chemin .pt du réseau NNUE")
    parser.add_argument("--model-type", required=True,
                        choices=["board768","halfka","halfkp","board768cuda","halfkacuda","halfkpcuda"],
                        help="Type de réseau (même choix que dans votre evaluate_model).")
    parser.add_argument("--scale", type=float, required=True,
                        help="Facteur d'échelle (même que dans evaluate_model).")
    parser.add_argument("--host", default="127.0.0.1", help="Adresse IP d'écoute")
    parser.add_argument("--port", type=int, default=5555, help="Port d'écoute")
    parser.add_argument("--max-clients", type=int, default=10, help="Nombre maximum de clients simultanés")
    args = parser.parse_args()

    # Sélection du type de réseau comme dans evaluate_model
    model_classes = {
        "board768":      NnBoard768,
        "board768cuda":  NnBoard768Cuda,
        "halfka":        NnHalfKA,
        "halfkacuda":    NnHalfKACuda,
        "halfkp":        NnHalfKP,
        "halfkpcuda":    NnHalfKPCuda,
    }
    model_class = model_classes[args.model_type]

    print(f"[Serveur] Chargement du réseau {args.model_type} depuis {args.model_path}")
    model = model_class(128).to(DEVICE)
    model.load_state_dict(torch.load(args.model_path, map_location=DEVICE))
    model.eval()
    print(f"[Serveur] Modèle chargé (device={DEVICE}), scale={args.scale}, écoute sur {args.host}:{args.port}\n")

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Permettre la réutilisation du port
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((args.host, args.port))
        s.listen(args.max_clients)
        print(f"[Serveur] En attente de connexions (max {args.max_clients} clients)...")
        
        # Liste des threads actifs
        client_threads = []
        
        try:
            while True:
                # Accepter une nouvelle connexion
                conn, addr = s.accept()
                print(f"[Serveur] Nouvelle connexion depuis {addr}")
                
                # Nettoyer les threads terminés
                client_threads = [t for t in client_threads if t.is_alive()]
                
                # Créer un nouveau thread pour gérer cette connexion
                client_thread = ClientHandler(conn, addr, model, args.scale)
                client_thread.daemon = True  # Pour que le thread se termine si le programme principal se termine
                client_thread.start()
                client_threads.append(client_thread)
                
                print(f"[Serveur] {len(client_threads)} client(s) actif(s)")
        except KeyboardInterrupt:
            print("\n[Serveur] Arrêt du serveur...")
        finally:
            # Attendre que tous les threads se terminent
            for t in client_threads:
                if t.is_alive():
                    t.join(1.0)  # Attendre au maximum 1 seconde
            print("[Serveur] Arrêt terminé")

if __name__ == "__main__":
    main()
