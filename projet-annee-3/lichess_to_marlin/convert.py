import json
import os
import sys
import zstandard as zstd
import requests
from datetime import datetime
import logging
from tqdm import tqdm
import io
import argparse

# Configuration du logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('logs/conversion.log'),
        logging.StreamHandler(sys.stdout)
    ]
)

class LichessConverter:
    def __init__(self):
        self.data_dir = "data"
        self.output_dir = "output"
        self.state_file = os.path.join(self.data_dir, "conversion_state.json")
        self.lichess_url = "https://database.lichess.org/eval/lichess_db_eval.jsonl.zst"
        self.lichess_file = os.path.join(self.data_dir, "lichess_db_eval.jsonl.zst")
        self.current_output_file = None
        self.positions_per_file = 1_000_000  # 1 million positions par fichier
        self.current_position_count = 0
        self.total_processed = 0
        self.load_state()

    def load_state(self):
        """Charge l'état précédent de la conversion si existant"""
        if os.path.exists(self.state_file):
            with open(self.state_file, 'r') as f:
                state = json.load(f)
                self.total_processed = state.get('total_processed', 0)
                self.current_position_count = state.get('current_position_count', 0)
        logging.info(f"État chargé: {self.total_processed} positions traitées au total")

    def save_state(self):
        """Sauvegarde l'état actuel de la conversion"""
        state = {
            'total_processed': self.total_processed,
            'current_position_count': self.current_position_count,
            'timestamp': datetime.now().isoformat()
        }
        with open(self.state_file, 'w') as f:
            json.dump(state, f, indent=2)
        logging.info(f"État sauvegardé: {self.total_processed} positions traitées")

    def download_if_needed(self):
        """Télécharge le fichier Lichess s'il n'existe pas"""
        try:
            if os.path.exists(self.lichess_file):
                # Vérifier si le fichier existant est valide
                try:
                    with open(self.lichess_file, 'rb') as fh:
                        dctx = zstd.ZstdDecompressor()
                        with dctx.stream_reader(fh) as reader:
                            # Lire les premiers octets pour vérifier si c'est un fichier zstd valide
                            reader.read(1)
                            return  # Le fichier est valide, on peut continuer
                except zstd.ZstdError:
                    logging.warning("Le fichier existant est corrompu, nouvelle tentative de téléchargement")
                    os.remove(self.lichess_file)

            logging.info("Téléchargement du fichier Lichess...")
            response = requests.get(self.lichess_url, stream=True)
            
            if response.status_code != 200:
                raise Exception(f"Erreur lors du téléchargement: {response.status_code}")
            
            total_size = int(response.headers.get('content-length', 0))
            
            # Créer le répertoire data s'il n'existe pas
            os.makedirs(self.data_dir, exist_ok=True)
            
            # Télécharger dans un fichier temporaire d'abord
            temp_file = self.lichess_file + '.tmp'
            try:
                with open(temp_file, 'wb') as f, tqdm(
                    desc="Téléchargement",
                    total=total_size,
                    unit='iB',
                    unit_scale=True
                ) as pbar:
                    for data in response.iter_content(chunk_size=8192):
                        size = f.write(data)
                        pbar.update(size)
                
                # Vérifier si le fichier téléchargé est un fichier zstd valide
                with open(temp_file, 'rb') as fh:
                    dctx = zstd.ZstdDecompressor()
                    with dctx.stream_reader(fh) as reader:
                        reader.read(1)  # Vérifier si on peut lire le début du fichier
                
                # Si tout est ok, renommer le fichier temporaire
                os.replace(temp_file, self.lichess_file)
                logging.info("Téléchargement terminé avec succès")
                
            except Exception as e:
                # En cas d'erreur, supprimer le fichier temporaire
                if os.path.exists(temp_file):
                    os.remove(temp_file)
                raise Exception(f"Erreur lors du téléchargement ou de la vérification: {str(e)}")
                
        except Exception as e:
            logging.error(f"Erreur fatale lors du téléchargement: {str(e)}")
            raise

    def get_output_filename(self):
        """Génère un nom de fichier de sortie basé sur le nombre de positions traitées"""
        file_number = self.total_processed // self.positions_per_file
        return os.path.join(self.output_dir, f"marlinflow_data_{file_number:04d}.txt")

    def evaluate_position(self, eval_data):
        """Convertit les données d'évaluation Lichess en format Marlinflow"""
        if not eval_data.get('evals'):
            return None

        # Prendre l'évaluation avec la plus grande profondeur
        best_eval = max(eval_data['evals'], key=lambda x: x.get('depth', 0))
        if not best_eval.get('pvs'):
            return None

        first_pv = best_eval['pvs'][0]
        
        # Obtenir l'évaluation en centipawns
        if 'cp' in first_pv:
            eval_cp = first_pv['cp']
        elif 'mate' in first_pv:
            # Convertir mat en une grande valeur de centipawns
            mate_score = first_pv['mate']
            eval_cp = 10000 if mate_score > 0 else -10000
        else:
            return None

        # Calculer le WDL (victoire/nulle/défaite)
        # Seuil de 200 centipawns pour considérer une position comme gagnante
        if eval_cp > 200:
            wdl = 1.0  # Victoire blanche
        elif eval_cp < -200:
            wdl = 0.0  # Victoire noire
        else:
            wdl = 0.5  # Nulle

        return {
            'fen': eval_data['fen'],
            'eval': eval_cp,
            'wdl': wdl
        }

    def process_positions(self, max_positions=None):
        """Traite les positions du fichier Lichess"""
        try:
            with open(self.lichess_file, 'rb') as fh:
                dctx = zstd.ZstdDecompressor()
                with dctx.stream_reader(fh) as reader:
                    text_stream = io.TextIOWrapper(reader, encoding='utf-8')
                    
                    # Configurer la barre de progression
                    pbar = tqdm(
                        total=max_positions if max_positions else None,
                        initial=0,
                        desc="Traitement des positions",
                        unit="pos"
                    )
                    
                    positions_processed = 0  # Compteur de positions réellement traitées
                    
                    for line_number, line in enumerate(text_stream, 1):
                        # Skip déjà traités
                        if line_number <= self.total_processed:
                            continue

                        try:
                            position_data = json.loads(line)
                            evaluated_pos = self.evaluate_position(position_data)
                            
                            if evaluated_pos:
                                # Ouvrir nouveau fichier si nécessaire
                                output_file = self.get_output_filename()
                                if output_file != self.current_output_file:
                                    if self.current_output_file:
                                        logging.info(f"Nouveau fichier de sortie: {output_file}")
                                    self.current_output_file = output_file

                                # Écrire la position
                                with open(output_file, 'a') as f:
                                    f.write(f"{evaluated_pos['fen']} | {evaluated_pos['eval']} | {evaluated_pos['wdl']}\n")
                                
                                self.current_position_count += 1
                                self.total_processed += 1
                                positions_processed += 1
                                pbar.update(1)

                                # Vérifier si on a atteint le nombre de positions demandé
                                if max_positions and positions_processed >= max_positions:
                                    logging.info(f"Nombre de positions demandé atteint ({max_positions})")
                                    break

                                # Sauvegarder l'état périodiquement
                                if self.total_processed % 10000 == 0:
                                    self.save_state()
                                    pbar.set_postfix({"total": self.total_processed})

                        except json.JSONDecodeError:
                            logging.error(f"Erreur JSON à la ligne {line_number}")
                            continue
                        except Exception as e:
                            logging.error(f"Erreur lors du traitement de la ligne {line_number}: {str(e)}")
                            continue

        except KeyboardInterrupt:
            logging.info("\nInterruption utilisateur détectée")
        finally:
            pbar.close()
            self.save_state()
            logging.info(f"Traitement terminé. Total positions traitées: {self.total_processed}")

def main():
    # Configuration des arguments en ligne de commande
    parser = argparse.ArgumentParser(description='Convertit la base de données Lichess en format Marlinflow')
    parser.add_argument('--positions', type=int,
                      help='Nombre de positions à traiter (optionnel, par défaut: traite tout jusqu\'à interruption)')
    parser.add_argument('--batch-size', type=int, default=10_000,
                      help='Taille du lot pour la sauvegarde de l\'état (défaut: 10 000)')
    args = parser.parse_args()

    if args.positions:
        logging.info(f"Démarrage du traitement pour {args.positions} positions")
    else:
        logging.info("Démarrage du traitement (continuera jusqu'à interruption)")
    logging.info("Pour interrompre proprement, utilisez Ctrl+C")
    
    try:
        converter = LichessConverter()
        converter.download_if_needed()
        converter.process_positions(max_positions=args.positions)  # None = pas de limite
    except KeyboardInterrupt:
        logging.info("\nInterruption demandée par l'utilisateur. Sauvegarde de l'état...")
    except Exception as e:
        logging.error(f"Erreur inattendue: {str(e)}")
    finally:
        logging.info("Programme terminé. Vous pouvez le relancer à tout moment, il reprendra où il s'est arrêté.")

if __name__ == "__main__":
    main() 