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


