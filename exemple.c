#include <stdio.h>
#include <stdint.h>
#include "bit_io.h"

#define BITS 9

int main() {

	FILE *fout = fopen("test.bin", "w");
	BIT_FILE *bfout = bit_begin(fout);
	/* we write three integers to the file, each using BITS bits */
	bit_put(bfout, 255, BITS);
	bit_put(bfout, 256, BITS);
	bit_put(bfout, 257, BITS);
	int size = bit_end(bfout);
	fclose(fout);

	printf("Number of bytes written: %d\n", size);

	FILE *fin = fopen("test.bin", "r");
	BIT_FILE *bfin = bit_begin(fin);
	uint32_t i;
	while (bit_get(bfin, &i, BITS) != EOF) {
		printf("Read: %d\n", i);
	}
	printf("End of file\n");
	size = bit_end(bfin);
	fclose(fin);

	printf("Number of bytes read: %d\n", size);

	return 0;
}
