#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit_io.h"
#include "arbre_dico.h"

#define CLEAR_CODE 256
#define END_CODE 257
#define NEXT_CODE 258
#define MAX_CODE_SIZE 12

#define BITS 12

#define TAILLE_CHN_TEMP 24

void compress(FILE *input_file,FILE *output_file) {
    BIT_FILE *bf = bit_begin(output_file);
    uint32_t next_code = NEXT_CODE;
    uint8_t input_symbol;
    uint32_t * chn;
    uint32_t * last_valid;
    int i;
    int j;
    int taille_last_valid=0;
    int taille_chn=0;
    uint32_t last_code = 0;
    int nombre_bits_courant = BITS;
    struct Trie* racine = CreationFeuille(-1);

    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));

    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        chn[0] = i;
        for (j=1;j<TAILLE_CHN_TEMP;j++)
            chn[j] = 0;
        insert(racine, chn ,i, 1);
    }  

    last_code = 0; 
    //fprintf(output_file,"%u ",256);
    bit_put(bf, 256, nombre_bits_courant);
     
    while ( ! feof(input_file)) { //tant que la fin du fichier n'est pas atteinte
        input_symbol = getc(input_file);
        if (input_symbol == 255 && feof(input_file)) break;//indicateurs de fin
        //printf("Lecture de %d\n",input_symbol);
        i=0;

        while(i<taille_last_valid)
            {
                chn[i] = last_valid[i] ;
                i++;  
            }
        chn[i] = input_symbol;
        i++;
        taille_chn = taille_last_valid + 1;    
        //printf("Taille_chn %d Taille_last %d\n",taille_chn,taille_last_valid);
        //printf("Recherche de %u %u %u %u %u %u %u %u %u %u \n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9]);
        //printf("Recherche code Last code %d\n",Recherche_code_dans_l_arbre(racine,chn,taille_chn));

        if (Recherche_code_dans_l_arbre(racine,chn,taille_chn) != -1) 
        {
            // Le code existe dans le dictionnaire
            memcpy(last_valid,chn,TAILLE_CHN_TEMP*sizeof(uint32_t));
            taille_last_valid = taille_chn;
            last_code = Recherche_code_dans_l_arbre(racine, chn, taille_chn);
            //printf("%u %u %u %u %u %u %u %u %u %u Trouvé! Last_valid %u %u %u Last_code %d  \n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],last_valid[0],last_valid[1],last_valid[2],last_code);
        } 
        else 
        {
            // Le code n existe pas: ajout au dictionnaire
            insert(racine, chn ,next_code, taille_chn);
            //printf("Ajouté %u %u %u %u %u %u %u %u %u %u en %d de taille %d\n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],next_code,taille_chn);
            next_code++;

            //printf("ECRITURE %u\n",last_code);
            bit_put(bf, last_code, nombre_bits_courant); 

            last_valid[0] = input_symbol;
            chn[0] = input_symbol;
            taille_last_valid = 1;
            taille_chn = 1;

            /*for (i=1;i<TAILLE_CHN_TEMP;i++){
                chn[i] = 0;
                last_valid[i] = 0;
            }*/
            last_code = input_symbol;
            //printf("%u %u %u %u %u %u %u %u %u %u Pas trouvé: Last valid %u %u %u Last code %d  Next code %d\n",chn[0],chn[1],chn[2],chn[3],chn[4],chn[5],chn[6],chn[7],chn[8],chn[9],last_valid[0],last_valid[1],last_valid[2],last_code,next_code);
        }
        
        
    }
    //fprintf(output_file,"%u ",last_code);
    bit_put(bf, last_code, nombre_bits_courant);
    //fprintf(output_file,"%u",END_CODE);
    bit_put(bf, END_CODE, nombre_bits_courant);

    bit_flush(bf);
    bit_end(bf);

    Liberation(racine);
    free(chn);
    free(last_valid);
}

void decompress(FILE *input_file,FILE *output_file) {

    uint32_t next_code = NEXT_CODE;
    int i, j;
    int currentNumber;
    uint32_t * last_valid;
    uint32_t * seq;
    uint32_t * chn;
    int taille_last_valid=0;
    int taille_seq=0;
    int taille_chn=1;
    int Trouve=0;
    uint32_t code;
    BIT_FILE *bf = bit_begin(input_file);
    struct Trie* racine = CreationFeuille(-1);
    int nombre_bits_courant = BITS;
    int PremierCaractere = 1;

    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    seq = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(seq,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        chn[0] = i;
        for (j=1;j<TAILLE_CHN_TEMP;j++)
            chn[j] = 0;
        insert(racine, chn ,i, 1);
    }  
    
   //une boucle qui prend chaque ligne les unes apres les autres jusqu'à la fin du fichier
    while (bit_get(bf, &code, nombre_bits_courant) != EOF) {
            currentNumber = code;

            if (currentNumber != 257 && currentNumber != 256)
            {
                //printf("Analyse du %d avec %d bits\n",currentNumber,nombre_bits_courant);
                memset(seq,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
                Trouve = Recherche_un_noeud(racine,currentNumber,*(&seq));
                //printf("retour de recherche seq %u %u Trouve %d\n",seq[0],seq[1],Trouve);
                taille_seq = CompteurNiveaux(racine, Recherche_dans_l_arbre(racine,currentNumber))-1;

                if (Trouve != 0){ //Trouvé

                    memset(chn,0,taille_chn * sizeof(uint32_t));    
                    memcpy(chn,last_valid,taille_last_valid*sizeof(uint32_t));
                    chn[taille_last_valid] = seq[0];
                    taille_chn = taille_last_valid + 1;
                    //printf("Taille chn %d\n",taille_chn);
                    if (PremierCaractere == 1) 
                    { 
                        PremierCaractere = 0; 
                    }
                    else
                    {
                        insert(racine,chn,next_code,taille_chn);
                        //printf("Ajout dico en:%d chn:%u %u %u %u taille %d\n",next_code,chn[0],chn[1],chn[2],chn[3],taille_chn);
                        next_code++;
                    }
                    
                    memset(last_valid,0,taille_last_valid * sizeof(uint32_t));
                    memcpy(last_valid,seq,taille_seq * sizeof(uint32_t)) ;
                    taille_last_valid = taille_seq;

                    for (i=0;i<taille_seq;i++)
                        {
                            //printf("ECRITURE %u\n", seq[i]);
                            fprintf(output_file,"%c", seq[i]); 
                        }  
                    //printf("%d Trouvé! last:%u %u %u seq:%u %u %u %u next:%d\n",currentNumber,last_valid[0],last_valid[1],last_valid[2],seq[0],seq[1],seq[2],seq[3],next_code);
                } 
                else //Pas trouvé
                {
                    memcpy(seq, last_valid, taille_last_valid * sizeof(uint32_t));  
                    seq[taille_last_valid] = last_valid[0];
                    taille_seq = taille_last_valid + 1;

                    memcpy(chn,last_valid,taille_last_valid*sizeof(uint32_t));   
                    chn[taille_last_valid] = seq[0];
                    taille_chn = taille_last_valid + 1;
                    insert(racine,chn,next_code, taille_chn);
                
                    //printf("Ajout dico next:%d chn:%u %u %u taille %d\n",next_code,chn[0],chn[1],chn[2],taille_chn);
                    next_code++;
                    /*if (next_code == 512) nombre_bits_courant = 10;
                    if (next_code == 1024) nombre_bits_courant = 11;
                    if (next_code == 2048) nombre_bits_courant = 12;*/
                    if (next_code == 4096) printf("Depassement de 12 bits\n");

                    memcpy(last_valid, seq, taille_seq * sizeof(uint32_t)) ; 
                    taille_last_valid = taille_seq;  
                    for (i=0;i<taille_seq;i++)
                    {
                        //printf("%u\n", seq[i]);
                        fprintf(output_file,"%c", seq[i]); 
                    }     
                    //printf("%d Pas Trouvé! last:%u %u %u seq:%u %u %u next:%d\n",currentNumber,last_valid[0],last_valid[1],last_valid[2],seq[0],seq[1],seq[2],next_code);
                }
            }       
    }

    bit_flush(bf);
    bit_end(bf);

    //printf("Nombre de noeuds : %d\n", CompteurNoeuds(racine));
    Liberation(racine);
    free(chn);
    free(last_valid);
    free(seq);
}

int main(int argc, char* argv[]) {
    FILE *input_file;
    FILE *output_file;
    char * output_file_name;

    if (argc > 2) {
    } else {
        printf("Format: ./LZW (encode or decode) NomFichier\n");
        exit(1);
    }

    if (strcmp(argv[1] , "encode") == 0 || strcmp(argv[1] , "decode") == 0) {
    }
    else {
        printf("Format: ./LZW (encode or decode) NomFichier\n");
        exit(1);
    }

    output_file_name = (char*)malloc(strlen(argv[2]) + 10);
    if (strcmp(argv[1] , "encode") == 0) strcpy(output_file_name, "encode_");
    else strcpy(output_file_name, "decode_");
    strcat(output_file_name, argv[2]);

    if (strcmp(argv[1] , "encode") == 0) {
        input_file = fopen(argv[2], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        }
        output_file = fopen(output_file_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        } 
        compress(input_file, output_file);

        fclose(input_file);
        fclose(output_file);
    }

    if (strcmp(argv[1] , "decode") == 0) {
        input_file = fopen(argv[2], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        }
        output_file = fopen(output_file_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        } 

        decompress(input_file,output_file);

        fclose(input_file);
        fclose(output_file);
    }

    free(output_file_name);
}