# Lichess to Marlinflow Converter

Ce script permet de convertir la base de données d'évaluations Lichess au format Marlinflow pour l'entraînement de réseaux de neurones.

## Structure du projet

```
lichess_to_marlin/
├── data/           # Stockage du fichier Lichess et de l'état
├── output/         # Fichiers de sortie au format Marlinflow
├── logs/           # Logs de conversion
├── convert.py      # Script principal
├── requirements.txt
└── README.md
```

## Installation

1. Créer un environnement virtuel Python:
```bash
python -m venv venv
source venv/bin/activate  # Linux/Mac
# ou
venv\Scripts\activate     # Windows
```

2. Installer les dépendances:
```bash
pip install -r requirements.txt
```

## Utilisation

### Mode par défaut
```bash
python convert.py
```
Le script va:
- Télécharger automatiquement le fichier Lichess s'il n'existe pas
- Convertir les positions jusqu'à ce que vous l'interrompiez avec Ctrl+C
- Sauvegarder l'état toutes les 10,000 positions
- Créer des fichiers de sortie de 1 million de positions chacun

### Options disponibles
```bash
python convert.py --positions 500000  # Traiter 500,000 positions puis s'arrêter
python convert.py --batch-size 20000  # Sauvegarder l'état toutes les 20,000 positions
```

## Gestion des interruptions

- Le script peut être interrompu à tout moment avec Ctrl+C
- L'état est sauvegardé automatiquement lors de l'interruption
- La reprise se fait automatiquement au redémarrage
- Les fichiers de sortie sont gérés par blocs de 1 million de positions

## Caractéristiques

- Reprise possible en cas d'interruption
- Logging détaillé dans `logs/conversion.log`
- Barre de progression pour le téléchargement et le traitement
- Gestion des erreurs
- Conversion intelligente des évaluations mate en centipawns
- Calcul approximatif du WDL (Win/Draw/Loss)

## Format de sortie

Chaque ligne du fichier de sortie est au format:
```
<fen> | <eval> | <wdl>
```
où:
- `<fen>` est la position d'échecs
- `<eval>` est l'évaluation en centipawns (point de vue blanc)
- `<wdl>` est la probabilité de victoire (1.0=victoire blanche, 0.5=nulle, 0.0=victoire noire)

## Notes

- Les positions sont filtrées pour ne garder que celles avec des évaluations valides
- Les évaluations mate sont converties en grandes valeurs de centipawns (±10000)
- Le WDL est calculé approximativement basé sur l'évaluation en centipawns
- Les fichiers de sortie sont nommés `marlinflow_data_XXXX.txt` où XXXX est un numéro séquentiel
- L'état de la conversion est sauvegardé dans `data/conversion_state.json` 