#CC := gcc
#CFLAGS := -Wall -pedantic 
#LDLIBS := -lm -Werror
#OBJ := bin/bit_io.o bin/arbre_dico.o bin/LZW.o bin/GIF.o
#MAIN := LZW

#all: LZW

#LZW.o: include/LZW.h bit_io.h src/bit_io.c include/arbre_dico.h src/arbre_dico.c src/LZW.c include/GIF.h GIF.c
#	$(CC) -c LZW.c $(CFLAGS)

#LZW: $(OBJ)
#	$(CC) -o $(MAIN) $(OBJ) $(LDLIBS)

#run: $(OBJ)
#	$(CC) -o $(MAIN) $(OBJ) $(LDLIBS)
#	./$(MAIN)

#clean: 
#	rm -f *.o
#	rm -f *~
#	rm -f LZW


#Constantes :
SP=src/
HD=include/
BLD=bin/
CC=gcc -g 
CFLAGS=-Wall
OBJ=${BLD}LZW.o ${BLD}arbre_dico.o ${BLD}bit_io.o ${BLD}GIF.o
LDFLAGS=-lm 
OUT=LZW


#Règle pour compiler le programme :
all: ${OBJ}
	${CC} -o ${OUT} ${OBJ} ${CFLAGS} ${LDFLAGS} 

#Règles de dépendances des fichiers sources :
${BLD}LZW.o: ${SP}LZW.c ${SP}arbre_dico.c ${SP}bit_io.c
	${CC} -c ${SP}LZW.c -o ${BLD}LZW.o ${CFLAGS} ${LDFLAGS}

${BLD}bit_io.o: ${SP}bit_io.c
	${CC} -c ${SP}bit_io.c -o ${BLD}bit_io.o ${CFLAGS} ${LDFLAGS}

${BLD}arbre_dico.o: ${SP}arbre_dico.c ${SP}bit_io.c 
	${CC} -c ${SP}arbre_dico.c -o ${BLD}arbre_dico.o ${CFLAGS} ${LDFLAGS}

${BLD}GIF.o: ${SP}GIF.c ${SP}arbre_dico.c ${SP}bit_io.c
	${CC} -c ${SP}GIF.c -o ${BLD}GIF.o ${CFLAGS} ${LDFLAGS}

#Règle pour effacer les fichiers compilés
clean :
	rm -f ${BLD}*
	rm -f ${OUT}

#Règle pour lancer le programme
run : ${OBJ}
	${CC} -o ${OUT} ${OBJ} ${CFLAGS} ${LDFLAGS}
	./${OUT}


