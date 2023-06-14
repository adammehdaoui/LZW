#ifndef GIF_H
#define GIF_H

#include "bit_io.h"
#include "arbre_dico.h"

#define MAX_ENCOD_GIF 255

/**
 * Compresse un fichier GIF en utilisant l'algorithme de compression LZW.
 *
 * @param bf             Pointeur vers une structure BIT_FILE pour écrire les données compressées.
 * @param input_file     Pointeur vers le fichier GIF d'entrée à compresser.
 * @param max_code_size  Taille maximale du code pour la compression LZW.
 */
int compressGIF(BIT_FILE * bf, FILE * input_file, int max_code_size, uint8_t * palette, struct Trie * racine, int current_code_size);

/**
 * Calcule la taille en bits du fichier compressé d'un GIF en utilisant l'algorithme de compression LZW.
 *
 * @param input_file     Pointeur vers le fichier GIF d'entrée à compresser.
 * @param max_code_size  Taille maximale du code pour la compression LZW.
 *
 * @return La taille en bits du fichier compressé.
 */
int amountCompressGIF(FILE * input_file, int max_code_size, uint8_t * palette);


/**
 * Crée un fichier GIF à partir d'un fichier d'entrée en utilisant l'algorithme de compression LZW.
 *
 * @param input         Pointeur vers le fichier d'entrée.
 * @param output        Pointeur vers le fichier de sortie GIF à créer.
 * @param max_code_size Taille maximale du code pour la compression LZW.
 */
void createGIF(FILE * input, FILE * output, int max_code_size) ;


#endif /* GIF_H */

