use std::fs::File;
use std::io::{BufRead, BufReader, BufWriter, Result, Write};
use std::path::PathBuf;

use cozy_chess::Board;
use marlinformat::PackedBoard;
use structopt::StructOpt;

/// Convert legacy text data format to marlinformat.
#[derive(StructOpt)]
pub struct Options {
    #[structopt(short, long)]
    output: PathBuf,

    txt_file: PathBuf,
}

pub fn run(options: Options) -> Result<()> {
    println!("Ouverture du fichier d'entrée : {:?}", options.txt_file);
    let input = BufReader::new(File::open(options.txt_file)?);
    println!("Ouverture du fichier de sortie : {:?}", options.output);
    let mut output = BufWriter::new(File::create(options.output)?);

    let mut had_non_integer_cp = false;
    let mut had_out_of_range_cp = false;
    let mut line_count = 0;
    let mut success_count = 0;

    for line in input.lines() {
        line_count += 1;
        let line = line?;
        
        // Afficher le progrès uniquement toutes les 100 000 lignes
        if line_count % 100_000 == 0 {
            println!("Traitement de la ligne {} : {}", line_count, line);
        }
        
        let result = (|| {
            let (board, annotation) = match line.split_once(" | ") {
                Some(parts) => parts,
                None => {
                    if line_count % 100_000 == 0 {
                        println!("  Erreur: Format de ligne invalide");
                    }
                    return None;
                }
            };
            
            let (cp, wdl) = match annotation.split_once(" | ") {
                Some(parts) => parts,
                None => {
                    if line_count % 100_000 == 0 {
                        println!("  Erreur: Format d'annotation invalide");
                    }
                    return None;
                }
            };

            let complete_fen = format!("{} 0 1", board);
            
            let board: Board = match complete_fen.parse() {
                Ok(b) => b,
                Err(e) => {
                    if line_count % 100_000 == 0 {
                        println!("  Erreur: FEN invalide - {:?}", e);
                    }
                    return None;
                }
            };

            let cp: f32 = match cp.parse() {
                Ok(v) => v,
                Err(e) => {
                    if line_count % 100_000 == 0 {
                        println!("  Erreur: CP invalide - {}", e);
                    }
                    return None;
                }
            };

            let wdl: f32 = match wdl.parse() {
                Ok(v) => v,
                Err(e) => {
                    if line_count % 100_000 == 0 {
                        println!("  Erreur: WDL invalide - {}", e);
                    }
                    return None;
                }
            };

            if !had_non_integer_cp && cp.floor() != cp {
                println!("Warning: dataset contains non-integer centipawn values. These will be truncated.");
                had_non_integer_cp = true;
            }

            let cp = match (cp as i64).try_into() {
                Ok(v) => v,
                Err(_) => {
                    if !had_out_of_range_cp {
                        println!("Warning: dataset contains centipawn values outside the range representable by an i16. These will be saturated.");
                        had_out_of_range_cp = true;
                    }
                    match cp.is_sign_positive() {
                        true => i16::MAX,
                        false => i16::MIN,
                    }
                },
            };

            let wdl = match () {
                _ if wdl < 0.25 => 0,
                _ if wdl < 0.75 => 1,
                _ => 2
            };
            
            if line_count % 100_000 == 0 {
                println!("  Valeurs converties - CP: {}, WDL: {}", cp, wdl);
                println!("  Position packée avec succès");
            }

            let packed = PackedBoard::pack(&board, cp, wdl, 0);
            Some(packed)
        })();

        if let Some(packed) = result {
            match output.write_all(bytemuck::bytes_of(&packed)) {
                Ok(_) => {
                    success_count += 1;
                    if success_count % 1_000_000 == 0 {
                        println!("  {} positions écrites avec succès", success_count);
                    }
                }
                Err(e) => println!("  Erreur lors de l'écriture: {}", e)
            }
        }
    }

    // Make sure to flush the buffer before closing
    output.flush()?;

    println!("Conversion terminée :");
    println!("  Lignes traitées : {}", line_count);
    println!("  Positions converties avec succès : {}", success_count);
    Ok(())
}
