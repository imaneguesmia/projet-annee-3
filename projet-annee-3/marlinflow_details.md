# Marlinflow - Guide d'utilisation approfondi

Marlinflow est un ensemble d'outils et de scripts permettant de **former des réseaux de neurones pour les moteurs d’échecs**, en particulier pour le moteur [Black Marlin](https://github.com/dsekercioglu/blackmarlin). Il inclut :

1. Des **kernels CUDA** spécialisés pour effectuer les multiplications sparse (sparse-coo-like) en GPU (fichier `cudasparse.py`),   
2. Un **parseur Rust** (généré en bibliothèque partagée `.so/.dll`) permettant de **lire des fichiers de données** au format Marlinflow (fichier `parse/` en Rust),  
3. Des **modules PyTorch** décrivant différentes têtes de réseau (par ex. `NnHalfKP`, `NnHalfKPCuda`, etc.) dans `model.py`,  
4. Un **DataLoader** Python (`dataloader.py`) permettant de charger batch après batch les données à partir de fichiers `.bin`,  
5. Un script principal de **training** (`trainer/main.py`) où est implémenté la boucle d’entraînement.

Grâce à ces éléments, vous pouvez :  
- Générer ou collecter des données (positions d’échecs, évaluation, WDL) sous le format Marlinflow,  
- Les convertir/ranger en gros fichiers `.bin` (ou convertir depuis/vers un format texte si nécessaire),  
- Les charger dans PyTorch via le DataLoader,  
- Entraîner un réseau au choix (par ex. HalfKP, Board768…) soit en CPU, soit en GPU (CUDA).  
- Sauvegarder périodiquement les états du réseau au format PyTorch (`.pt`) et/ou JSON, afin de les utiliser dans votre moteur ou de continuer l’entraînement plus tard.

---

## 1. Prérequis

- **Python 3**  
- **PyTorch** (pour l’entraînement des réseaux)  
- **NumPy**  
- **Cargo (Rust)** : pour compiler la librairie de parsing (`libparse.so` / `libparse.dll`)  
- **cupy** (optionnel) : utilisé pour compiler et exécuter les kernels CUDA personnalisés de `cudasparse.py` (si vous souhaitez la partie GPU).  
- Un GPU NVIDIA (optionnel, mais recommandé) : si vous voulez utiliser la version CUDA/GPUs (sinon, tout fonctionne en CPU).

---

## 2. Installation et compilation

1. **Clonez** ce dépôt :  
   ```bash
   git clone https://github.com/dsekercioglu/marlinflow
   ```
   Rendez-vous ensuite dans le répertoire cloné :  
   ```bash
   cd marlinflow
   ```

2. **Compilez** la bibliothèque de parsing (en Rust) dans le dossier `parse/`. Cela génèrera un fichier `.so` (Linux) ou `.dll` (Windows). Vous pouvez également ajouter `-C target-cpu=native` pour utiliser les optimisations CPU natives :  
   ```bash
   cd parse
   cargo rustc --release -- -C target-cpu=native
   ```
   Après compilation, vous devriez voir un fichier dans `target/release/` appelé `libparse.so` (Linux) ou `libparse.dll` (Windows).

3. **Déplacez** ce fichier `.so/.dll` dans le répertoire `trainer/`, en le renommant en `libparse.so` (sous Linux) ou `libparse.dll` (sous Windows) :  
   ```bash
   mv target/release/libparse.so ../trainer/
   ```
   *(ou le fichier *.dll si vous êtes sous Windows)*  

4. Dans `trainer/`, créez quelques dossiers utiles :  
   ```bash
   mkdir nn
   mkdir runs
   ```
   - Le dossier `nn/` contiendra les checkpoints sauvés (réseau au format PyTorch ainsi que `.json`).  
   - Le dossier `runs/` contiendra les logs d’entraînement.

5. **Vérifiez** que votre environnement Python contient bien PyTorch, NumPy et cupy (si vous utilisez CUDA) :  
   ```bash
   pip install torch numpy cupy
   ```
   ou via conda/mamba :
   ```bash
   conda install pytorch numpy cupy-cudaXX -c pytorch
   ```

---

## 3. Préparation des données

### 3.1 Formats de données

Marlinflow utilise un **format binaire** propriétaire (plus compact) pour l’entraînement. Vous pouvez :  
- Soit **générer directement** ce format dans votre propre code de génération de données (fortement recommandé),  
- Soit convertir des données depuis un **format texte “legacy”** grâce à `marlinflow-utils`.

Le format texte “legacy” liste, pour chaque ligne :  
```
<fen> | <eval> | <wdl>
```
- `<fen>` : FEN complet de la position,  
- `<eval>` : évaluation en centipions (du point de vue des blancs),  
- `<wdl>` : 1.0 (victoire blancs), 0.5 (nul), ou 0.0 (victoire noirs).

### 3.2 Conversion depuis le format texte “legacy”

Si vous avez des données en format texte, vous pouvez les convertir en `.bin` :  
1. Compilez l’outil de conversion :  
   ```bash
   cd utils
   cargo rustc --release -- -C target-cpu=native
   ```
2. Exécutez-le pour créer un fichier `.bin` :  
   ```bash
   target/release/marlinflow-utils txt-to-data INPUT.txt --output OUTPUT.bin
   ```
3. Une fois vos fichiers `.bin` prêts, placez-les dans un dossier dédié (par ex. `trainer/data`).

---

## 4. Lancement de l'entraînement

Vous utiliserez généralement le script `main.py` situé dans `trainer/`. Voici un exemple d’invocation :

```bash
cd trainer
python main.py       \
  --data-root data   \
  --train-id net0001 \
  --lr 0.001         \
  --epochs 45        \
  --lr-drop 30       \
  --batch-size 16384 \
  --wdl 0.3          \
  --scale 400        \
  --save-epochs 5
```

### 4.1 Paramètres importants

- `--data-root`: dossier où se trouvent les fichiers `.bin`. (ex: `trainer/data`)  
- `--train-id`: identifiant utilisé pour sauvegarder votre modèle (ex: `nn/net0001.json`, `nn/net0001_5`, etc.).  
- `--lr`: learning rate initial (par ex. 0.001).  
- `--epochs`: nombre d’époques (passes) sur l’ensemble de vos données.  
- `--lr-drop`: époch à partir de laquelle le learning rate est réduit par un facteur 10. (par ex. si `--lr-drop 30`, alors après 30 époques, le lr passe de 0.001 à 0.0001).  
- `--batch-size`: taille de batch (ex: 16384). Ajustez en fonction de la mémoire disponible.  
- `--wdl`: pondération entre la prédiction d’évaluation “classique” (0) et la prédiction du résultat WDL (1.0).  
   - 0.0 => le réseau se concentre uniquement sur l’évaluation en centipions  
   - 1.0 => le réseau est entraîné uniquement pour la probabilité de gain / nul / perte  
   - Entre les deux => un mix des deux signaux d’entraînement  
- `--scale`: facteur de mise à l’échelle pour le *sigmoid* (concerne la partie éval en centipions). Par exemple `400` => la sortie finale en centipions est “compromisée” par un `sigmoid(x/400)`.  
- `--save-epochs`: fréquence de sauvegarde du modèle (ex: toutes les 5 époques).  

### 4.2 Déroulement de l’entraînement

1. **Chargement des binaires** : le script lit les fichiers `.bin` dans `--data-root`, via la classe `BatchLoader`.  
2. **Entraînement** : à chaque batch, le script :  
   - Forward pass : calcule la sortie du réseau (eval + wdl selon la pondération)  
   - Calcule la perte MSE (Mean Squared Error) entre la sortie prédisée et la cible.  
   - Backpropagation + optimisation (Adam)  
   - Clip des poids (`WeightClipper`) pour éviter des valeurs extrêmes (ex: ±1.98).  
3. **Suivi et Logging** :  
   - Toutes les `LOG_ITERS` positions (défini dans le code, par ex. 10 millions), un log s’affiche et s’enregistre dans `runs/<train-id>.txt`.  
   - À la fin de chaque époque, un rapport donne la perte moyenne et le débit (positions/s).  
4. **Sauvegarde** : toutes les `--save-epochs` époques, on sauvegarde :  
   - Un *checkpoint* PyTorch : `nn/<train-id>_<epoch>` (state_dict)  
   - Un *fichier JSON* : `nn/<train-id>.json` contenant les poids finaux (lisible pour la conversion NNUE ou autre).

### 4.3 Exécution CPU ou GPU ?

- **GPU** : Par défaut, `main.py` crée un modèle `NnHalfKPCuda(...)` s’il détecte une GPU NVIDIA et si vous avez `cupy` installé.  
- **CPU** : Pour utiliser la version CPU, vous pouvez modifier le modèle instancié (par ex. utiliser `NnHalfKP(128)` au lieu de `NnHalfKPCuda(128)` dans `main.py`).  
- Les classes de modèles se trouvent dans `model.py`. Exemple :  
  - `NnBoard768` / `NnBoard768Cuda`  
  - `NnHalfKP` / `NnHalfKPCuda`  
  - `NnHalfKA` / `NnHalfKACuda`  

---

## 5. Continuer ou reprendre un entraînement (“incrémental”)

### 5.1 Reprise depuis un checkpoint PyTorch

Pour entraîner votre réseau “par morceaux” (par ex. commencer un entraînement, l’interrompre, puis le reprendre plus tard avec d’autres fichiers de données), il suffit de **recharger** le `state_dict` du réseau sauvegardé. 

**Exemple minimal :**

```python
import torch
from model import NnHalfKPCuda

model = NnHalfKPCuda(128)
model.load_state_dict(torch.load("nn/net0001_5"))  # Charge le checkpoint d'epoch 5

# On peut créer un nouvel optimizer ou reprendre l'ancien état si vous l'avez sauvegardé.
optimizer = torch.optim.Adam(model.parameters(), lr=0.0001)

# Ensuite, vous pouvez relancer l'entraînement :
# (code identique à ce que fait trainer/main.py)
```

> **Remarque** : Par défaut, `main.py` ne sauvegarde pas l'état de l'optimiseur. Vous pouvez donc continuer avec un *nouvel* optimiseur, ou modifier `main.py` pour qu’il fasse aussi `torch.save(optimizer.state_dict(), ...)` si vous souhaitez tout reprendre exactement là où vous vous êtes arrêté.

### 5.2 Ajouter de nouvelles données

Si vous avez de nouveaux `.bin` (d’autres positions), vous pouvez simplement :

1. Les placer dans le dossier `data/` (ou un autre dossier),  
2. Lancer à nouveau `main.py` (ou un script perso) en chargeant le réseau déjà partiellement entraîné,  
3. Ajouter tous les chemins de fichiers `.bin` à votre DataLoader pour qu’il les lise.

---

## 6. Conversion du réseau pour un moteur d’échecs

En fin d’entraînement, vous obtenez un fichier JSON (ex: `nn/net0001.json`) qui contient tous les poids.  
- **Quantification** (optionnelle) : Pour un usage en moteur d’échecs, on compresse souvent les poids en 8 bits.  
- **Conversion** :  
  - Pour un réseau *NNUE de type “perspective”* (ex: Stockfish-like), vous pouvez utiliser [nnue-jsontobin](https://github.com/cosmobobak/nnue-jsontobin).  
  - Pour d’autres structures (HalfKP, HalfKA, etc.), il vous faudra créer/adapter un convertisseur approprié. Un point de départ se trouve dans `marlinflow-utils convert`, qui peut convertir un JSON NNUE en format binaire “BlackMarlin NNUE” (uniquement HalfKP pour l’instant).

---

## 7. Organisation du code

Pour mieux comprendre chaque fichier :

- **`trainer/main.py`** : Script principal d’entraînement. Parse les arguments, instancie le modèle (souvent `NnHalfKPCuda`), lance la boucle d’apprentissage.  
- **`model.py`** : Contient différentes classes PyTorch pour les réseaux (ex: `NnHalfKP`, `NnHalfKPCuda`, etc.).  
- **`cudasparse.py`** : Implémente des *kernels* CUDA personnalisés pour les couches “sparse” (DoubleFeatureTransformerSlice, etc.). Gère forward/backward en accumulant rapidement les features.  
- **`dataloader.py`** : Code permettant de **lire le fichier `.so/.dll` Rust** (`libparse`), d’appeler le parseur en C, et de récupérer sous forme de `torch.Tensor` (CPU/GPU) les features, eval, WDL. Gère le chargement batch par batch (`BatchLoader`).  
- **`parse/`** (dossier Rust) : Code pour construire `libparse.so` / `libparse.dll`. Fournit les API de parsing en C (via FFI).  
- **`utils/`** : Contient `marlinflow-utils` (en Rust) qui propose diverses commandes (conversion `txt->data`, `shuffle`, `interleave`, `convert`, etc.).  
- **`trainlog.py`** : Petite classe pour loguer les pertes (loss) dans un fichier `runs/<train-id>.txt`.  

---

## 8. Astuces & Conseils

1. **Shuffle** : Avant l’entraînement, il est très important de mélanger (shuffle) les données, surtout si elles proviennent de sources différentes ou suivent un ordre particulier.  
   - Vous pouvez utiliser `marlinflow-utils shuffle input.bin --output output_shuffled.bin` pour mélanger un fichier.  
   - Ou bien `marlinflow-utils interleave` pour fusionner plusieurs fichiers déjà mélangés.

2. **Batch size** : Ajustez selon la mémoire. Trop grand => risque de *out of memory*. Trop petit => temps d’entraînement plus long.

3. **Monitoring** : Jetez un œil régulier à la perte (loss) et aux logs dans `runs/<train-id>.txt`. Vous pouvez tracer ces courbes avec un outil (par ex. `matplotlib` ou `tensorboard`).

4. **Limite de la couche** : Les couches GPU (“Cuda”) sont spécialisées HalfKP, Board768, HalfKA, etc. Vérifiez que votre format de features correspond bien à celle attendue (l’ID du feature set se voit dans `dataloader.py`).

5. **Garder l’historique** : Pour un véritable *resume training*, vous voudrez probablement aussi sauvegarder l’état de l’optimiseur. Vous pouvez facilement modifier `main.py` pour faire :  
   ```python
   torch.save({
       'model': model.state_dict(),
       'optimizer': optimizer.state_dict()
   }, f"nn/{train_id}_checkpoint_{epoch}")
   ```
   Puis recharger plus tard avec `checkpoint = torch.load(...)`.

---

## 9. Exemples d’utilisation

### 9.1 Entraîner un HalfKP en GPU

```bash
python main.py \
  --data-root ./data \
  --train-id halfkp001 \
  --lr 0.001 \
  --epochs 40 \
  --batch-size 32768 \
  --wdl 0.5 \
  --scale 400 \
  --save-epochs 5 \
  --lr-drop 20
```
- Démarre l’entraînement du réseau `NnHalfKPCuda(128)` (par défaut dans `main.py`) en lisant tous les `.bin` dans `./data`.  
- Sauvegarde tous les 5 epochs.  
- Diminue le LR x10 après 20 époques.  

### 9.2 Continuer l’entraînement sur un nouveau dataset

1. Placez les nouveaux `.bin` dans `data2/`.  
2. Chargez un checkpoint précédent (`nn/halfkp001_40`) :

   ```python
   import torch
   from model import NnHalfKPCuda
   from train import train  # Supposez que votre fonction train() est accessible

   model = NnHalfKPCuda(128)
   model.load_state_dict(torch.load("nn/halfkp001_40"))
   optimizer = torch.optim.Adam(model.parameters(), lr=0.0001)

   # Créez un BatchLoader pour data2/...
   # Entraînez, logguez, etc.
   ```

3. Lancez un nouveau script (similaire à `main.py`) qui utilise ce checkpoint, un nouveau `DataLoader` pointant vers `data2/`, etc.

---

## 10. FAQ

**Q** : *Est-ce que je peux utiliser un autre type de feature ?*  
**R** : Oui, mais il faudra probablement coder un nouveau module `NnXxx` et ajuster la partie Rust parseur, ou adapter la partie CUDA dans `cudasparse.py`.  

**Q** : *Comment convertir le JSON final pour mon moteur ?*  
**R** : Voyez [nnue-jsontobin](https://github.com/cosmobobak/nnue-jsontobin) pour un réseau NNUE “standard perspective”. Pour HalfKP/KA, vous pouvez regarder `marlinflow-utils convert` (ou coder votre propre convertisseur).

---

## 11. Références

- **Marlinflow** : <https://github.com/dsekercioglu/marlinflow>  
- **Black Marlin** : <https://github.com/dsekercioglu/blackmarlin>  
- **Viridithas** : <https://github.com/cosmobobak/viridithas>  
- **Svart** : <https://github.com/crippa1337/svart>  
- **Carp** : <https://github.com/dede1751/carp>  
- **nnue-jsontobin** : <https://github.com/cosmobobak/nnue-jsontobin>

---
