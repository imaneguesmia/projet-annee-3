#!/usr/bin/env python3
"""
Script pour préparer les données de test en convertissant un fichier texte en format binaire.
"""

import os
import argparse
import subprocess
import pathlib

def main():
    parser = argparse.ArgumentParser(description="Prépare les données de test pour l'évaluation")
    parser.add_argument("--input-file", type=str, required=True, 
                        help="Fichier texte d'entrée à convertir")
    parser.add_argument("--output-dir", type=str, default="test_data",
                        help="Répertoire de sortie pour les données de test")
    
    args = parser.parse_args()
    
    # Créer le répertoire de sortie s'il n'existe pas
    output_dir = pathlib.Path(args.output_dir)
    output_dir.mkdir(exist_ok=True, parents=True)
    
    # Chemin du fichier de sortie
    output_file = output_dir / "test_data.bin"
    
    # Chemin absolu vers le répertoire utils
    utils_dir = pathlib.Path(__file__).parent.parent / "utils"
    
    # Vérifier que le fichier d'entrée existe
    if not os.path.exists(args.input_file):
        print(f"Erreur: Le fichier d'entrée {args.input_file} n'existe pas.")
        return
    
    print(f"Conversion du fichier {args.input_file} en format binaire...")
    
    # Construire la commande pour exécuter l'outil txt-to-data
    cmd = [
        "cargo", "run", "--bin", "marlinflow-utils", "--", 
        "txt-to-data", 
        args.input_file,
        "--output", str(output_file)
    ]
    
    # Exécuter la commande depuis le répertoire utils
    try:
        process = subprocess.run(
            cmd, 
            cwd=utils_dir,
            check=True,
            capture_output=True,
            text=True
        )
        print(process.stdout)
        
        if os.path.exists(output_file):
            file_size = os.path.getsize(output_file) / (1024 * 1024)  # Taille en Mo
            print(f"Conversion réussie! Fichier créé: {output_file} ({file_size:.2f} Mo)")
        else:
            print("Erreur: Le fichier de sortie n'a pas été créé.")
            
    except subprocess.CalledProcessError as e:
        print(f"Erreur lors de la conversion: {e}")
        print(f"Sortie standard: {e.stdout}")
        print(f"Erreur standard: {e.stderr}")
    
if __name__ == "__main__":
    main() 