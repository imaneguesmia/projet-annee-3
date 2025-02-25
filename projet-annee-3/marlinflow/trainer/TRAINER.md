# Guide d'utilisation du Trainer Marlinflow

## Table des matières
1. [Fonctionnalités principales](#fonctionnalités-principales)
2. [Utilisation de base](#utilisation-de-base)
3. [Paramètres d'entraînement](#paramètres-dentraînement)
4. [Gestion des checkpoints](#gestion-des-checkpoints)
5. [Système de logs](#système-de-logs)
6. [Modifications récentes](#modifications-récentes)

## Fonctionnalités principales

Le trainer permet de :
- Entraîner un réseau de neurones pour l'évaluation de positions d'échecs
- Sauvegarder des checkpoints réguliers du modèle
- Reprendre un entraînement interrompu
- Suivre la progression via des logs détaillés
- Combiner évaluation en centipawns (CP) et prédiction de victoire/nulle/défaite (WDL)

## Utilisation de base

### Premier entraînement
```bash
python main.py \
  --train-id mon_modele \
  --data-root data \
  --lr 0.001 \
  --epochs 100 \  # Fera 100 epochs au total
  --batch-size 16384 \
  --wdl 0.1 \
  --scale 400 \
  --save-epochs 10
```

### Continuer un entraînement
```bash
# Continuer automatiquement depuis le dernier checkpoint
# Si le dernier checkpoint est à l'epoch 100 et qu'on spécifie epochs=150,
# cela fera 50 epochs supplémentaires (de 100 à 150)
python main.py \
  --train-id mon_modele \
  --data-root data \
  --lr 0.001 \
  --epochs 150 \  # Nombre TOTAL d'epochs souhaité
  --batch-size 16384 \
  --wdl 0.1 \
  --scale 400 \
  --save-epochs 10

# OU reprendre depuis un checkpoint spécifique
python main.py \
  --train-id mon_modele \
  --continue-from nn/mon_modele_80 \  # Reprendra à partir de l'epoch 80
  --epochs 150 \  # Fera 70 epochs supplémentaires (de 80 à 150)
  --...
```

## Paramètres d'entraînement

| Paramètre | Description | Valeur par défaut |
|-----------|-------------|-------------------|
| `--train-id` | Identifiant unique de l'entraînement | Requis |
| `--data-root` | Dossier contenant les fichiers .bin | Requis |
| `--lr` | Learning rate initial | Requis |
| `--epochs` | Nombre TOTAL d'epochs souhaité (pas le nombre supplémentaire) | Requis |
| `--batch-size` | Taille des batchs | 16384 |
| `--wdl` | Poids de la prédiction WDL (0-1) | 0.0 |
| `--scale` | Échelle pour la sigmoid | Requis |
| `--save-epochs` | Fréquence de sauvegarde | 100 |
| `--lr-drop` | Epoch où diviser lr par 10 | None |
| `--continue-from` | Checkpoint spécifique | Auto |

## Gestion des checkpoints

Le système utilise deux formats de sauvegarde différents :

### 1. Fichiers de checkpoint (format binaire)
- Nom : `nn/train_id_epoch` (exemple : `nn/mon_modele_100`)
- Un nouveau fichier créé tous les N epochs (`--save-epochs`)
- Format binaire PyTorch
- Utilisés pour :
  - Reprendre l'entraînement
  - Garder un historique des différentes versions
- Conservés indéfiniment (sauf suppression manuelle)

### 2. Fichier JSON (format texte)
- Nom : `nn/train_id.json` (exemple : `nn/mon_modele.json`)
- Un seul fichier qui est mis à jour à chaque sauvegarde
- Format JSON lisible et portable
- Contient toujours la dernière version du modèle
- Utilisé pour :
  - Exporter le modèle vers le moteur d'échecs
  - Analyser les poids du réseau
  - Convertir vers d'autres formats
- Écrasé à chaque sauvegarde avec la dernière version

### Fréquence de sauvegarde
Les deux fichiers sont mis à jour en même temps :
```python
if epoch % save_epochs == 0:
    # Sauvegarde binaire
    torch.save(model.state_dict(), f"nn/{train_id}_{epoch}")
    # Sauvegarde JSON
    save_json(model, f"nn/{train_id}.json")
```

### Lors de la reprise d'entraînement
1. Le système charge le dernier checkpoint binaire
2. Continue l'entraînement
3. Met à jour le fichier JSON avec les nouveaux poids à chaque sauvegarde

## Système de logs

Les logs sont stockés dans `runs/train_id.txt` avec le format :
```
Training Parameters (Started: 2024-02-24 20:45:00)
--------------------------------------------------------------------------------
learning_rate: 0.001
epochs: 150
batch_size: 16384
wdl_weight: 0.1
scale: 400
save_epochs: 10
lr_drop: None
device: cuda:0
architecture: NnHalfKP(128)

Training Log
--------------------------------------------------------------------------------
epoch: 0 - loss: 0.00990143
epoch: 1 - loss: 0.00426520
...
```

Les logs incluent :
- Les paramètres d'entraînement
- La date de début
- L'évolution de la loss par epoch
- La fréquence des logs est contrôlée par `LOG_ITERS` (500,000 positions)

## Modifications récentes

### Améliorations des logs
- Ajout des paramètres d'entraînement en en-tête
- Vrais numéros d'epochs dans les logs
- Format plus lisible
- Préservation de l'historique entre les sessions

### Gestion des checkpoints
- Détection automatique du dernier checkpoint
- Option pour charger un checkpoint spécifique
- Reprise correcte de l'epoch de départ

### Autres améliorations
- Logs plus fréquents (tous les 500k positions)
- Meilleurs messages de progression
- Documentation complète

## Comprendre la Loss

### Calcul de la Loss
```python
# Prédiction du réseau (entre 0 et 1)
prediction = model(batch)

# Valeur attendue (entre 0 et 1)
expected = torch.sigmoid(batch.cp / scale) * (1 - wdl) + batch.wdl * wdl

# Erreur quadratique moyenne
loss = torch.mean((prediction - expected) ** 2)
```

### Interprétation des valeurs
- La loss est une erreur quadratique moyenne (MSE)
- Les valeurs sont toujours entre 0 et 1 grâce à la normalisation
- Pour interpréter une valeur de loss :
  - Prenez la racine carrée pour avoir l'erreur moyenne
  - Multipliez par scale (400) pour avoir l'erreur en centipawns

Exemples :
| Loss    | Erreur moyenne | Erreur en centipawns |
|---------|---------------|---------------------|
| 0.02000 | 0.141        | ~56 cp             |
| 0.01000 | 0.100        | ~40 cp             |
| 0.00250 | 0.050        | ~20 cp             |
| 0.00016 | 0.013        | ~5 cp              |

### Évolution typique
- Début (~0.01) : Le réseau fait des erreurs de 30-40 centipawns
- Milieu (~0.002) : Les erreurs diminuent à 15-20 centipawns
- Fin (~0.0005) : Le réseau devient précis à 5-10 centipawns près

### Composantes de la Loss
La loss combine deux objectifs :
1. **Évaluation en centipawns (CP)** :
   - Normalisée par `sigmoid(cp / scale)`
   - Poids : `(1 - wdl)` (ex: 0.9 si wdl=0.1)

2. **Prédiction WDL** :
   - Valeurs : 0 (défaite), 0.5 (nulle), 1 (victoire)
   - Poids : `wdl` (ex: 0.1)


