from __future__ import annotations

import argparse
import pathlib
import torch
from dataloader import BatchLoader
from model import NnBoard768, NnHalfKA, NnHalfKP, NnBoard768Cuda, NnHalfKACuda, NnHalfKPCuda

DEVICE = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

def evaluate_model(
    model: torch.nn.Module,
    dataloader: BatchLoader,
    wdl: float,
    scale: float,
) -> dict:
    """
    Évalue un modèle entraîné sur un ensemble de données
    Retourne un dictionnaire avec différentes métriques
    """
    model.eval()  # Met le modèle en mode évaluation
    total_mse = 0.0
    total_samples = 0
    total_correct_predictions = 0  # Pour calculer la précision
    
    # Pour suivre la distribution des erreurs
    error_ranges = {
        "0-0.1": 0,
        "0.1-0.2": 0,
        "0.2-0.3": 0,
        "0.3-0.4": 0,
        "0.4-1.0": 0,
        "1.0+": 0 
    }
    
    # Pour calculer l'erreur en centipawns
    total_cp_error = 0.0
    
    with torch.no_grad():
        while True:
            is_new_epoch, batch = dataloader.read_batch(DEVICE)
            if is_new_epoch:
                break
                
            prediction = model(batch)
            expected = torch.sigmoid(batch.cp / scale) * (1 - wdl) + batch.wdl * wdl
            
            # Calcul MSE
            mse = torch.mean((prediction - expected) ** 2)
            total_mse += mse.item() * batch.size
            
            # Calcul des erreurs absolues
            errors = torch.abs(prediction - expected)
            for err in errors:
                if err < 0.1:
                    error_ranges["0-0.1"] += 1
                elif err < 0.2:
                    error_ranges["0.1-0.2"] += 1
                elif err < 0.3:
                    error_ranges["0.2-0.3"] += 1
                elif err < 0.4:
                    error_ranges["0.3-0.4"] += 1
                elif err < 1.0:
                    error_ranges["0.4-1.0"] += 1
                else:
                    error_ranges["1.0+"] += 1
            
            # Calcul précision (considère une prédiction comme correcte si erreur < 0.1)
            total_correct_predictions += torch.sum(errors < 0.1).item()
            
            # Calcul de l'erreur en centipawns
            # Convertir les prédictions en centipawns
            pred_cp = -torch.log(1.0 / prediction - 1.0) * scale
            expected_cp = batch.cp
            cp_errors = torch.abs(pred_cp - expected_cp)
            total_cp_error += torch.sum(cp_errors).item()
            
            total_samples += batch.size
    
    # Calcul des métriques finales
    avg_mse = total_mse / total_samples
    accuracy = total_correct_predictions / total_samples
    avg_cp_error = total_cp_error / total_samples
    
    # Normalisation de la distribution des erreurs
    for key in error_ranges:
        error_ranges[key] = error_ranges[key] / total_samples * 100  # en pourcentage

    return {
        "mse": avg_mse,
        "accuracy": accuracy,
        "avg_cp_error": avg_cp_error,
        "error_distribution": error_ranges,
        "total_positions": total_samples
    }

def main():
    parser = argparse.ArgumentParser(description="Évalue un modèle entraîné sur des données de test")
    
    parser.add_argument("--model-path", type=str, required=True, help="Chemin vers le modèle entraîné")
    parser.add_argument("--model-type", type=str, required=True, 
                       choices=["board768", "halfka", "halfkp", "board768cuda", "halfkacuda", "halfkpcuda"],
                       help="Type de modèle à évaluer")
    parser.add_argument("--test-data", type=str, required=True, help="Dossier contenant les données de test")
    parser.add_argument("--batch-size", type=int, default=16384, help="Taille du batch")
    parser.add_argument("--wdl", type=float, default=0.0, help="Poids WDL")
    parser.add_argument("--scale", type=float, required=True, help="Facteur d'échelle")
    
    args = parser.parse_args()

    # Création du modèle selon le type spécifié
    model_classes = {
        "board768": NnBoard768,
        "halfka": NnHalfKA,
        "halfkp": NnHalfKP,
        "board768cuda": NnBoard768Cuda,
        "halfkacuda": NnHalfKACuda,
        "halfkpcuda": NnHalfKPCuda
    }
    
    model_class = model_classes[args.model_type]
    model = model_class(128).to(DEVICE)
    
    # Chargement des poids du modèle
    print(f"Chargement du modèle depuis {args.model_path}")
    model.load_state_dict(torch.load(args.model_path))
    
    # Préparation des données de test
    test_path = pathlib.Path(args.test_data)
    test_files = list(map(str, test_path.glob("*.bin")))
    
    if not test_files:
        raise ValueError("Aucun fichier de test trouvé!")
    
    print(f"Nombre de fichiers de test trouvés : {len(test_files)}")
    
    # Création du dataloader
    dataloader = BatchLoader(test_files, model.input_feature_set(), args.batch_size)
    
    # Évaluation
    print("Début de l'évaluation...")
    results = evaluate_model(model, dataloader, args.wdl, args.scale)
    
    # Affichage des résultats
    print("\nRésultats de l'évaluation:")
    print(f"Nombre total de positions évaluées: {results['total_positions']}")
    print(f"MSE moyenne: {results['mse']:.6f}")
    print(f"Précision (erreur < 0.1): {results['accuracy']*100:.2f}%")
    print(f"Erreur moyenne en centipawns: {results['avg_cp_error']:.2f}")
    
    print("\nDistribution des erreurs:")
    for range_name, percentage in results['error_distribution'].items():
        print(f"Erreur {range_name}: {percentage:.2f}%")

if __name__ == "__main__":
    main() 