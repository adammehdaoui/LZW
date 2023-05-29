#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit_io.h"
#include "arbre_dico.h"

#define CLEAR_CODE 256
#define END_CODE 257
#define NEXT_CODE 258
#define MAX_CODE_SIZE 12

#define NULL_CODE_9BITS 255
#define NULL_CODE_10BITS 511
#define NULL_CODE_11BITS 1023
#define NULL_CODE_12BITS 2047

#define BITS 12

#define TAILLE_CHN_TEMP 24

void compress(FILE *input_file,FILE *output_file) {
    BIT_FILE *bf = bit_begin(output_file);

    uint32_t next_code = NEXT_CODE;
    uint8_t input_symbol;
    uint32_t chn[TAILLE_CHN_TEMP];
    uint32_t last_valid[TAILLE_CHN_TEMP];
    int i;
    int j;
    int taille;
    uint32_t last_code = 0;
    int nombre_bits_courant = BITS;
    int compteur_lignes = 0;

    struct Trie* head = CreationFeuille(-1);

    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        chn[0] = i;
        for (j=1;j<TAILLE_CHN_TEMP;j++)
            chn[j] = 0;
        insert(head, chn ,i);
    }  
    
    last_code = 0; 
    //fprintf(output_file,"%u ",256);
    bit_put(bf, 256, nombre_bits_courant);
    
    while ( ! feof(input_file)) { //while not end of file do
        input_symbol = getc(input_file);
        compteur_lignes++;
        //printf("input_symbol %d\n",input_symbol);
        if (input_symbol == 255) break;//indicateur de fin
        if (input_symbol == 0) input_symbol = NULL_CODE_9BITS;//indicateur de fin
        //printf("Lecture de %d ligne %d\n",input_symbol,compteur_lignes);
        i=0;

        taille = quelle_taille(last_valid);
        while(i<taille)
            {
                chn[i] = last_valid[i] ;
                i++;  
            }
        chn[i] = input_symbol;
        i++;
        while(i<TAILLE_CHN_TEMP)
            {
                chn[i] = 0 ;
                i++;  
            }

        //printf("Recherche de %u %u %u %u %u %u %u %u %u %u code Rech 2 %d\n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],Recherche_code_dans_l_arbre(head,chn));

        if (Recherche(head,chn) == 1) 
        {
            // Le code existe dans le dictionnaire
            memcpy(last_valid,chn,TAILLE_CHN_TEMP*sizeof(uint32_t));
            last_code = Recherche_code_dans_l_arbre(head,chn);
            //printf("%u %u %u %u %u %u %u %u %u %u Trouvé! Last_valid %u %u %u Last_code %d  \n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],last_valid[0],last_valid[1],last_valid[2],last_code);
        } 
        else 
        {
            // La sequence n existe pas: ajout au dictionnaire
            insert(head, chn ,next_code);
            //printf("Ajouté %u %u %u %u %u %u %u %u %u %u en %d\n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],next_code);
            next_code++;
            //printf("next_code %d\n",next_code);
            /*if (next_code == 512) nombre_bits_courant = 10;
            if (next_code == 1024) nombre_bits_courant = 11;
            if (next_code == 2048) nombre_bits_courant = 12;*/
            if (next_code == 4096) printf("Depassement de 12 bits\n");

            //printf("%u ",last_code);
            bit_put(bf, last_code, nombre_bits_courant); 

            last_valid[0] = input_symbol;
            chn[0] = input_symbol;
            for (i=1;i<TAILLE_CHN_TEMP;i++){
                chn[i] = 0;
                last_valid[i] = 0;
            }
            last_code = Recherche_code_dans_l_arbre(head,chn);
            //printf("%u %u %u %u %u %u %u %u %u %uPas trouvé: Last valid %u %u %u Last code %d  Next code %d\n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],last_valid[0],last_valid[1],last_valid[2],last_code,next_code);
        }
    }
    //fprintf(output_file,"%u ",last_code);
    bit_put(bf, last_code, nombre_bits_courant);
    //fprintf(output_file,"%u",END_CODE);
    bit_put(bf, END_CODE, nombre_bits_courant);

    bit_flush(bf);
    bit_end(bf);

    Liberation(head);
}

void decompress(FILE *input_file,FILE *output_file) {

    uint32_t next_code = NEXT_CODE;
    int i, j;
    int currentNumber;
    uint32_t last_valid[TAILLE_CHN_TEMP];
    uint32_t seq[TAILLE_CHN_TEMP];
    uint32_t chn[TAILLE_CHN_TEMP];
    int Trouve=0;
    uint32_t code;
    BIT_FILE *bf = bit_begin(input_file);
    struct Trie* head = CreationFeuille(-1);
    int nombre_bits_courant = BITS;
    int CompteurLignes = 0;

    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(seq,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        chn[0] = i;
        for (j=1;j<TAILLE_CHN_TEMP;j++)
            chn[j] = 0;
        insert(head, chn ,i);
    }  
    
   //une boucle qui prend chaque ligne les unes apres les autres jusqu'à la fin du fichier
    while (bit_get(bf, &code, nombre_bits_courant) != EOF) {
            currentNumber = code;
            CompteurLignes++;

            if (currentNumber != 257 && currentNumber != 256)
            {
                //printf("Analyse du %d avec %d bits en ligne %d\n",currentNumber,nombre_bits_courant,CompteurLignes);
                memset(seq,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
                Trouve = Recherche_un_noeud(head,currentNumber,*(&seq));
                
                if (Trouve != 0){ //Trouvé
                    if (last_valid[0] != 0) {
                        memset(chn,0,quelle_taille(chn)*sizeof(uint32_t));    
                        memcpy(chn,last_valid,quelle_taille(last_valid)*sizeof(uint32_t));
                        chn[quelle_taille(last_valid)] = seq[0];
                        //printf("Taille %d\n",taille);
                        insert(head,chn,next_code);
                        //printf("Ajout dico en:%d last:%u %u %u %u\n",next_code,chn[0],chn[1],chn[2],chn[3]);
                        next_code++;
                        /*if (next_code == 512) nombre_bits_courant = 10;
                        if (next_code == 1024) nombre_bits_courant = 11;
                        if (next_code == 2048) nombre_bits_courant = 12;*/
                        if (next_code == 4096) printf("Depassement de 12 bits\n");
                    }
                    
                    memset(last_valid,0,quelle_taille(last_valid)*sizeof(uint32_t));
                    memcpy(last_valid,seq,quelle_taille(seq)*sizeof(uint32_t)) ;

                    for (i=0;i<quelle_taille(seq);i++)
                        {
                            //printf("%u\n", seq[i]);
                            if (seq[i] != 255)
                                fprintf(output_file,"%c", seq[i]); 
                            else
                                fprintf(output_file,"%c", 0);
                        }  
                    //printf("%d Trouvé! last:%u %u %u seq:%u %u %u %u next:%d\n",currentNumber,last_valid[0],last_valid[1],last_valid[2],seq[0],seq[1],seq[2],seq[3],next_code);
                } 
                else //Pas trouvé
                {
                    memcpy(seq,last_valid,quelle_taille(last_valid)*sizeof(uint32_t));  
                    seq[quelle_taille(last_valid)] = last_valid[0];

                    memcpy(chn,last_valid,quelle_taille(last_valid)*sizeof(uint32_t));   
                    chn[quelle_taille(last_valid)] = seq[0];
                    insert(head,chn,next_code);
                    //printf("Ajout dico next:%d last:%u %u %u\n",next_code,chn[0],chn[1],chn[2]);
                    next_code++;
                    /*if (next_code == 512) nombre_bits_courant = 10;
                    if (next_code == 1024) nombre_bits_courant = 11;
                    if (next_code == 2048) nombre_bits_courant = 12;*/
                    if (next_code == 4096) printf("Depassement de 12 bits\n");

                    memcpy(last_valid,seq,quelle_taille(seq)*sizeof(uint32_t)) ;   
                    for (i=0;i<quelle_taille(seq);i++)
                    {
                        //printf("%u\n", seq[i]);
                        if (seq[i] != 255)
                            fprintf(output_file,"%c", seq[i]); 
                        else
                            fprintf(output_file,"%c", 0);
                    }     
                    //printf("%d Pas Trouvé! last:%u %u %u seq:%u %u %u next:%d\n",currentNumber,last_valid[0],last_valid[1],last_valid[2],seq[0],seq[1],seq[2],next_code);
                }
            }       
    }

    bit_flush(bf);
    bit_end(bf);

    printf("Nombre de noeuds : %d\n", CompteurNoeuds(head));
    Liberation(head);
}

int main() {
    FILE *input_file = fopen("comp_input.txt", "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
    FILE *output_file = fopen("comp_output.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    } 
    compress(input_file, output_file);

    fclose(input_file);
    fclose(output_file);
    printf("--------------------\n");
    input_file = fopen("comp_output.txt", "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
    output_file = fopen("decomp_output.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    } 

    decompress(input_file,output_file);

    fclose(input_file);
    fclose(output_file);
}