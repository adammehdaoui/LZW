#include "../include/bit_io.h"
#include <stdlib.h>
#include <fcntl.h>

BIT_FILE *bit_begin(FILE *f) {
    BIT_FILE *bf = (BIT_FILE *)malloc(sizeof(BIT_FILE));
    bf->fp = f;
    bf->buffer = 0;
    bf->bufpos = 0;
    bf->status = 0;
    bf->nb_bytes = 0;
    return bf;
}

void bit_flush(BIT_FILE *bf);

int bit_end(BIT_FILE *bf) {
    int n;

    if ((fcntl(fileno(bf->fp), F_GETFL) & O_ACCMODE) == O_WRONLY) {
        bit_flush(bf);
    }
    n = bf->nb_bytes;
    free(bf);
    return n;
}

int bit_eof(BIT_FILE *bf) {
    return bf->status == EOF;
}

void bit_put(BIT_FILE *bf, uint32_t data, int nbits) {

    /* add bits to the buffer */
    bf->buffer |= (data << bf->bufpos);
    /*fprintf(stderr, "buffer: %lx\n", bf->buffer); */
    bf->bufpos += nbits;
    /*fprintf(stderr, "bufpos: %d\n", bf->bufpos); */

    /* write full bytes from the buffer to the stream */
    while (bf->bufpos >= 8) {
        uint8_t byte = bf->buffer & 0xff;
        /*fprintf(stderr, "byte: %x\n", byte); */
        putc(byte, bf->fp);
        bf->nb_bytes++;

        /* remove byte from buffer and adjust bufpos */
        bf->buffer >>= 8;
        /*fprintf(stderr, "buffer: %lx\n", bf->buffer); */
        bf->bufpos -= 8;
        /* fprintf(stderr, "bufpos: %d\n", bf->bufpos); */
    }
}

void bit_flush(BIT_FILE *bf) {

    /* write final bits padded with 0s to a full byte */
    if (bf->bufpos != 0) {
        uint8_t byte = bf->buffer & 0xff;
        putc(byte, bf->fp);
        bf->nb_bytes++;
        bf->buffer = 0;
        bf->bufpos = 0;
    }
}

int bit_get(BIT_FILE *bf, uint32_t *data, int nbits) {

    int extracted;

    if (bf->status == EOF)
        return EOF;

    while (bf->bufpos < nbits) {
        int input = getc(bf->fp);
        if (input == EOF) {
            bf->status = EOF;
            return EOF;
        }
        uint8_t byte = input;
        bf->nb_bytes++;

        bf->buffer |= (uint64_t)byte << bf->bufpos;
        bf->bufpos += 8;
    }

    extracted = nbits;

    *data = bf->buffer & ((INT64_C(1) << extracted)-1);

    /* remove bits from buffer and adjust bufpos */
    bf->buffer >>= extracted;
    bf->bufpos -= extracted;

    return extracted;
}
