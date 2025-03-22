#!/bin/bash

# Script pour préparer les données de test et exécuter l'évaluation

# Vérifier si le modèle est spécifié
if [ -z "$1" ]; then
    echo "Usage: $0 <chemin_du_modèle> [type_de_modèle]"
    echo "Exemple: $0 nn/beluga_v1_10 halfkp"
    exit 1
fi

MODEL_PATH=$1
MODEL_TYPE=${2:-"halfkp"}  # Par défaut, utiliser halfkp
TEST_FILE="/Users/lucaferrari/CLionProjects/projet-annee-3/projet-annee-3/lichess_to_marlin/output/marlinflow_data_0102.txt"
TEST_DATA_DIR="test_data"

# Vérifier si le fichier de test existe
if [ ! -f "$TEST_FILE" ]; then
    echo "Erreur: Le fichier de test $TEST_FILE n'existe pas."
    exit 1
fi

# Vérifier si le modèle existe
if [ ! -f "$MODEL_PATH" ]; then
    echo "Erreur: Le modèle $MODEL_PATH n'existe pas."
    exit 1
fi

# Créer le répertoire de test s'il n'existe pas
mkdir -p "$TEST_DATA_DIR"

echo "=== Préparation des données de test ==="
python prepare_test_data.py --input-file "$TEST_FILE" --output-dir "$TEST_DATA_DIR"

# Vérifier si la conversion a réussi
if [ ! -f "$TEST_DATA_DIR/test_data.bin" ]; then
    echo "Erreur: La conversion des données de test a échoué."
    exit 1
fi

echo ""
echo "=== Exécution de l'évaluation ==="
python evaluate.py --model-path "$MODEL_PATH" --model-type "$MODEL_TYPE" --test-data "$TEST_DATA_DIR" --scale 400 --wdl 0.0

echo ""
echo "Évaluation terminée!" 