void compress(FILE *fpIn, FILE *fpOut);

void decompress(FILE *fpIn, FILE *fpOut);

#define CURRENT_MAX_CODES(bits)     ((unsigned int)(1 << (bits)))
