#ifndef GIF_H
#define GIF_H

#include <stdint.h>
#include <stdio.h>
#include "bit_io.h"

void new_gif(    uint16_t width, uint16_t height, uint8_t *palette, BIT_FILE *bf);
void close_gif(BIT_FILE *bf);
void createGIF(FILE * input, FILE * output, int max_code_size) ;
int amountCompressGIF(FILE * input_file, int max_code_size);

#endif /* GIF_H */

