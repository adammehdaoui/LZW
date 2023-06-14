#ifndef LZW_H
#define LZW_H

#define NBSP_CODE 255
#define CLEAR_CODE 256
#define END_CODE 257
#define NEXT_CODE 258

#define MIN_CODE_SIZE 9

#define TAILLE_CHN_TEMP 1024

/**
 * @brief Compresse un fichier en utilisant l'algorithme de compression LZW.
 *
 * Cette fonction compresse le fichier d'entrée en utilisant l'algorithme de compression spécifié. Elle écrit la version compressée du fichier dans le fichier de sortie.
 *
 * @param input_file Le fichier d'entrée à compresser.
 * @param output_file Le fichier de sortie où écrire la version compressée du fichier.
 * @param max_code_size La taille maximale en bits des codes écrits dans le fichier de sortie.
 */
void compress(FILE *fpIn, FILE *fpOut, int max_code_size);

/**
 * @brief Décompresse un fichier en utilisant l'algorithme de décompression LZW.
 *
 * Cette fonction décompresse le fichier d'entrée en utilisant l'algorithme de décompression spécifié. 
 * Elle écrit la version décompressée du fichier dans le fichier de sortie.
 *
 * @param input_file Le fichier d'entrée à décompresser.
 * @param output_file Le fichier de sortie où écrire la version décompressée du fichier.
 * @param max_code_size La taille maximale en bits des codes de compression.
 */
void uncompress(FILE *fpIn, FILE *fpOut, int max_code_size);

#endif
