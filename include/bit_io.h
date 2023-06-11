#ifndef BIT_IO_H
#define BIT_IO_H
#include <stdio.h>
#include <stdint.h>

/*
    A wrapper for a file pointer that allows reading/writing
    a variable number of bits (8..32) from/to a binary file.
*/
typedef struct _bit_file {
    /* A file pointer */
    FILE *fp;

    /* A buffer holding partial data from the stream */
    uint64_t buffer;

    /* The current number of bits in the buffer */
    int bufpos;

    /* The status of the file, 0 = ok, 1 = eof */
    int status;

    /* Number of bytes read/written */
    int nb_bytes;
} BIT_FILE;

/* Initialise the writing to *f using mode 'r' or 'w'. */
BIT_FILE *bit_begin(FILE *f);

/* Return the total number of bytes read/written. */
int bit_end(BIT_FILE *bf);

/* Write data to the file using nbits bits. */
void bit_put(BIT_FILE *bf, uint32_t data, int nbits);

void bit_flush(BIT_FILE *bf);

/* Read nbits to *data from the file.
   Return the number of bits read or EOF. */
int bit_get(BIT_FILE *bf, uint32_t *data, int nbits);

/* Return 1 if eof has been detected, 0 otherwise. */
int bit_eof(BIT_FILE *bf);

#endif /* BIT_IO_H */
