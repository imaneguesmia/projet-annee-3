#!/bin/bash

# Fichier de sortie
OUTPUT_FILE="combined/combined_positions.txt"

# Vérifier si le fichier de sortie existe déjà et le supprimer
if [ -f "$OUTPUT_FILE" ]; then
    rm "$OUTPUT_FILE"
    echo "Ancien fichier combiné supprimé."
fi

echo "Début de la combinaison des 100 premiers fichiers..."
echo "Cette opération peut prendre plusieurs minutes en raison de la taille des fichiers."

# Combiner les 100 premiers fichiers (de 0000 à 0099)
for i in $(seq -f "%04g" 0 99); do
    FILE="output/marlinflow_data_${i}.txt"
    if [ -f "$FILE" ]; then
        echo "Ajout du fichier $FILE..."
        cat "$FILE" >> "$OUTPUT_FILE"
    else
        echo "Attention: Le fichier $FILE n'existe pas et sera ignoré."
    fi
done

# Compter le nombre de lignes dans le fichier combiné
LINE_COUNT=$(wc -l < "$OUTPUT_FILE")
SIZE=$(du -h "$OUTPUT_FILE" | cut -f1)

echo "Combinaison terminée!"
echo "Le fichier combiné contient $LINE_COUNT lignes."
echo "Taille du fichier: $SIZE"
echo "Fichier sauvegardé sous: $OUTPUT_FILE" 