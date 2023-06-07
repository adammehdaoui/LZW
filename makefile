CC := gcc
CFLAGS := -Wall -pedantic 
LDLIBS := -lm -Werror
OBJ := bit_io.o arbre_dico.o LZW.o GIF.o
MAIN := LZW

all: LZW

LZW.o: LZW.h bit_io.h bit_io.c arbre_dico.h arbre_dico.c LZW.c GIF.h GIF.c
	$(CC) -c LZW.c $(CFLAGS)

LZW: $(OBJ)
	$(CC) -o $(MAIN) $(OBJ) $(LDLIBS)

run: $(OBJ)
	$(CC) -o $(MAIN) $(OBJ) $(LDLIBS)
	./$(MAIN)

clean: 
	rm -f *.o
	rm -f *~
	rm -f LZW
