CC := gcc
CFLAGS := -Wall 
LDLIBS := -lm -Werror
OBJ := bit_io.o LZW.o
MAIN := LZW

all: LZW

LZW.o: LZW.h bit_io.h bit_io.c LZW.c 
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
