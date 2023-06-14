#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bit_io.h"
#include "../include/arbre_dico.h"
#include "../include/LZW.h"
#include "../include/GIF.h"
#include <math.h>


void compress(FILE *input_file, FILE *output_file, int max_code_size) {
    BIT_FILE *bf = bit_begin(output_file);
    int next_code = NEXT_CODE;
    uint8_t input_symbol;
    uint32_t * chn;
    uint32_t * last_valid;
    int i;
    int taille_last_valid=0;
    int taille_chn=0;
    int last_code = 0;
    int current_code_size = MIN_CODE_SIZE;
    struct Trie* racine = dictionaryInitialization();

    /* Alloue la mémoire et initialise chn et last_valid */
    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    /* Initialise last_code */
    last_code = 0; 

    /* Ecrit le code CLEAR_CODE dans le fichier de sortie */
    bit_put(bf, CLEAR_CODE, current_code_size); 
     
    /* tant que le fichier d'entrée n'est pas terminé */
    while ( ! feof(input_file)) { 
        input_symbol = getc(input_file);
        
        /* si le caractère lu est NBSP_CODE(255) et que le fichier d'entrée est terminé, on sort de la boucle */
        if (input_symbol == NBSP_CODE && feof(input_file)) break;
        
        /* chn = last_valid + input_symbol */
        i=0;
        while(i<taille_last_valid)
        {
            chn[i] = last_valid[i] ;
            i++;  
        }
        chn[i] = input_symbol;
        i++;
        taille_chn = taille_last_valid + 1;  
        
        /* Recherche de chn dans le dictionnaire */
        if (findCodeTree(racine,chn,taille_chn) != -1) 
        {   /* Le code existe dans le dictionnaire */

            /* last_valid = chn */
            memcpy(last_valid,chn,TAILLE_CHN_TEMP*sizeof(uint32_t));
            taille_last_valid = taille_chn;

            /* last_code recherché dans le dictionnaire */
            last_code = findCodeTree(racine, chn, taille_chn);
        } 
        else  
        {   /* Le code n existe pas: ajout au dictionnaire */

            /* Ecriture de last_code dans le fichier de sortie */
            bit_put(bf, last_code, current_code_size); 
            
            /* Si le dictionnaire n'est pas plein au regard du nombre de bits de codage courant */
            /* Alors on ajoute chn au dictionnaire */
            if (! (next_code == pow(2,current_code_size) && current_code_size == max_code_size)) {
                insert(racine, chn ,next_code, taille_chn);
                next_code++;
                
                /* on ne peut plus ajouter de code dans le dictionnaire
                   alors il faut augmenter le nombre de bits de codage */
                if (next_code == pow(2,current_code_size)+1) {
                    current_code_size++;
                }    
            }
            /* Sinon on réinitialise le dictionnaire
               on réinitialise les variables associées, 
               le nombre de bits de codage courant et le prochain code à ajouter */
            else
            {
                /* Ecriture de CLEAR_CODE dans le fichier de sortie */
                bit_put(bf, CLEAR_CODE, current_code_size);
                
                /* Réinitialisation des variables */
                current_code_size = MIN_CODE_SIZE;
                next_code = NEXT_CODE;
                
                /* Réinitialisation du dictionnaire */
                freeTree(racine);
                racine = dictionaryInitialization();
            }

            /* last_valid = input_symbol */
            last_valid[0] = input_symbol;
            taille_last_valid = 1;

            /* last_code = input_symbol */
            last_code = input_symbol;
        }
      
    }
    
    /* Ecriture de last_code et END_CODE (257) dans le fichier de sortie */
    bit_put(bf, last_code, current_code_size); 
    bit_put(bf, END_CODE, current_code_size); 

    /* Libération et fermeture de bf */
    bit_flush(bf);
    bit_end(bf);

    /* Liberation de l'arbre et des chaines */
    freeTree(racine);
    free(chn);
    free(last_valid);
}

void uncompress(FILE *input_file,FILE *output_file, int max_code_size) {

    int next_code = NEXT_CODE;
    int i;
    int currentNumber;
    uint32_t * last_valid;
    uint32_t * seq;
    uint32_t * chn;
    int taille_last_valid=0;
    int taille_seq=0;
    int taille_chn=1;
    uint32_t code;
    BIT_FILE *bf = bit_begin(input_file);
    int current_code_size = MIN_CODE_SIZE;
    int * * dictionnaire;

    /* Alloue la mémoire de chn, last_valid et seq */
    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    seq = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));

    /* Alloue la mémoire du dictionnaire en fonction de la taille maximale des codes
       Pour Max_code_size = 12, on a 4096 codes possibles
       Pour Max_code_size = 20, on a 1048576 codes possibles */
    dictionnaire = (int * *)malloc(pow(2,max_code_size)*sizeof(int*));
    for (i = 0; i < pow(2,max_code_size); i++) {
        dictionnaire[i] = (int*)malloc(TAILLE_CHN_TEMP*sizeof(int));
        memset(dictionnaire[i],0,TAILLE_CHN_TEMP*sizeof(int));
    }

    /* Initialise les chaines d'entiers chn, last_valid et seq à 0 */
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(seq,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    
    /* tant que le fichier d'entrée n'est pas terminé */
    while (bit_get(bf, &code, current_code_size) != EOF) {
        currentNumber = code;
        
        /* Si le code lu est CLEAR_CODE (256)
           Alors on réinitialise le dictionnaire à 0
           on initialise le dictionnaire avec les codes ASCII de 0 à 256 */
        if (currentNumber == CLEAR_CODE) {
            
            /* Réinitialisation du dictionnaire */
            for (i = 0; i < pow(2,max_code_size); i++) {
                memset(dictionnaire[i],0,TAILLE_CHN_TEMP*sizeof(int));
            }

            /* Initialisation du dictionnaire avec les codes ascii de 0 à 256 */
            for (i = 0; i < CLEAR_CODE; i++) {
                dictionnaire[i][0] = i;
                dictionnaire[i][TAILLE_CHN_TEMP-1] = 1;
            } 

            /* Réinitialisation des variables */
            next_code = NEXT_CODE;
            current_code_size = MIN_CODE_SIZE;
            memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
            taille_last_valid = 0;

            /* On continue la lecture du fichier */
            continue;
        }

        /* Si le code lu est END_CODE (257), on reprend la boucle */
        if (currentNumber == END_CODE) break;

        /* Initialisation de la chaine seq à 0 */
        memset(seq,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
            
        /* Si le code lu est inférieur au prochain code à ajouter dans le dictionnaire
           Alors on récupère la chaine correspondante dans le dictionnaire */
        if (currentNumber < next_code){ 
            memcpy(seq, dictionnaire[currentNumber], TAILLE_CHN_TEMP * sizeof(uint32_t)) ;
            taille_seq = dictionnaire[currentNumber][TAILLE_CHN_TEMP-1];
        } 
        /* Sinon on récupère la chaine seq = last_valid + last_valid[0] */
        else { 
            memcpy(seq, last_valid, taille_last_valid * sizeof(uint32_t));  
            seq[taille_last_valid] = last_valid[0];
            taille_seq = taille_last_valid + 1;
        }    

        /* Ecriture de seq dans le fichier de sortie */
        for (i=0;i<taille_seq;i++)
            fprintf(output_file,"%c", seq[i]); 
        
        /* chn = last_valid + seq[0] */
        memset(chn,0,taille_chn * sizeof(uint32_t));
        memcpy(chn,last_valid,taille_last_valid*sizeof(uint32_t));   
        chn[taille_last_valid] = seq[0];
        taille_chn = taille_last_valid + 1;

        /* Si la chaine last_valid n'est pas vide
           Alors on ajoute chn au dictionnaire */
        if (taille_last_valid != 0){ 
            /* Vérification que le prochain code à ajouter dans le dictionnaire n'est pas une puissance de 2
               Si c'est le cas, on quitte le programme */
            if (pow(2,max_code_size) == next_code){
                printf("Veuillez vérifier le MAX_CODE_SIZE passé en paramètre\n");
                exit(1);
            }

            /* Ajout de chn au dictionnaire à l'emplacement next_code */
            for (i=0;i<taille_chn;i++)
                    dictionnaire[next_code][i] = chn[i];
            /* En bout de chaine, on stocke la taille de la chaine (Gestion du 0) */        
            dictionnaire[next_code][TAILLE_CHN_TEMP-1] = taille_chn;
            next_code++;

            /* Si le prochain code à ajouter dans le dictionnaire est une puissance de 2
               Alors on augmente le nombre de bits de codage courant
               Tant que le nombre de bits de codage courant est inférieur à la taille maximale des codes */
            if (next_code == pow(2,current_code_size) && current_code_size < max_code_size) {
                current_code_size++;
            }
            
        }

        /* last_valid = seq */     
        memset(last_valid,0,taille_last_valid * sizeof(uint32_t));
        memcpy(last_valid, seq, taille_seq * sizeof(uint32_t)) ; 
        taille_last_valid = taille_seq;  
 
    }  

    if (bit_get(bf, &code, current_code_size) > 0)
        printf("Veuillez vérifier le MAX_CODE_SIZE passé en paramètre\n");
                
    /* Libération et fermeture de bf */
    bit_flush(bf);
    bit_end(bf);

    /* Liberation de l'arbre et des tableaux */
    free(chn);0
    free(last_valid);
    free(seq);

    /* Libération du dictionnaire */
    for (i = 0; i < pow(2,max_code_size); i++) {
        free(dictionnaire[i]);
    }
    free(dictionnaire);
}

int main(int argc, char* argv[]) {
    FILE *input_file;
    FILE *output_file;
    char * output_file_name;
    char chn_argv2[2];

    /* Si le nombre d'arguments est différent de 4
       Alors on affiche le format d'appel du programme
       et on quitte le programme */
    if (argc != 4) {
        printf("Format: ./LZW (encode / decode / gif) (-maxBYTES) NomFichier\nexemple: ./LZW encode -max20 MonFichier.txt\n");
        exit(1);
    }

    /* Si le premier argument est différent de "encode", "decode" ou "gif"
       Alors on affiche le format d'appel du programme
       et on quitte le programme */
    if (!(strcmp(argv[1] , "encode") == 0 || strcmp(argv[1] , "decode") == 0 || strcmp(argv[1] , "gif") == 0)) {
        printf("Format: ./LZW (encode / decode / gif) (-maxBYTES) NomFichier\nexemple: ./LZW encode -max20 MonFichier.txt\n");
        exit(1);
    }

    /* Si le deuxième argument est dans "-max12" à "-max20"
       Alors on récupère le nombre de bits de codage
       Sinon on affiche le format d'appel du programme
       et on quitte le programme */
    if (strcmp(argv[2], "-max12") == 0 || strcmp(argv[2], "-max13") == 0 || strcmp(argv[2], "-max14") == 0 || strcmp(argv[2], "-max15") == 0 || strcmp(argv[2], "-max16") == 0 || strcmp(argv[2], "-max17") == 0 || strcmp(argv[2], "-max18") == 0 || strcmp(argv[2], "-max19") == 0 || strcmp(argv[2], "-max20") == 0 ) {
        chn_argv2[0] = argv[2][4];
        chn_argv2[1] = argv[2][5];
    }
    else {
        printf("Format: ./LZW (encode / decode / gif) (-maxBYTES) NomFichier\nexemple: ./LZW encode -max20 MonFichier.txt\n");
        exit(1);
    }
    /* On initialise le nom du fichier de sortie
       en fonction du premier argument et du nom du fichier d'entrée
       On alloue la mémoire pour le nom du fichier de sortie */
    output_file_name = (char*)malloc(strlen(argv[3]) + 10);
    if (strcmp(argv[1] , "encode") == 0) strcpy(output_file_name, "encode_");
    if (strcmp(argv[1] , "decode") == 0)  strcpy(output_file_name, "decode_");
    if (strcmp(argv[1] , "gif") == 0)  strcpy(output_file_name, "gif_");
    strcat(output_file_name, argv[3]);

    /* Si le premier argument est "encode"
       Alors on ouvre le fichier d'entrée en lecture binaire
       et on ouvre le fichier de sortie en écriture */
    if (strcmp(argv[1] , "encode") == 0) {
        /* On ouvre le fichier d'entrée en lecture binaire */
        input_file = fopen(argv[3], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        }

        /* On ouvre le fichier de sortie en écriture */
        output_file = fopen(output_file_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        } 
        
        /* On compresse le fichier d'entrée
           en utilisant l'algorithme de compression LZW
           et on écrit la version compressée du fichier dans le fichier de sortie
           en utilisant le nombre de bits de codage spécifié */
        compress(input_file, output_file, atoi(chn_argv2));

        /* On ferme les fichiers d'entrée et de sortie */
        fclose(input_file);
        fclose(output_file);
    }    

    /***************************************************************/
    
    if (strcmp(argv[1] , "gif") == 0) {
        
        /* On ouvre le fichier d'entrée en lecture binaire */
        input_file = fopen(argv[3], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        }

        /* On ouvre le fichier de sortie en écriture */
        output_file = fopen(output_file_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        } 
        
        createGIF(input_file, output_file, atoi(chn_argv2));

        fclose(input_file);
        fclose(output_file);
    }

    /*********************************************************************/

    /* Si le premier argument est "decode"
       Alors on ouvre le fichier d'entrée en lecture binaire
       et on ouvre le fichier de sortie en écriture */
    if (strcmp(argv[1] , "decode") == 0) {
        /* On ouvre le fichier d'entrée en lecture binaire */
        input_file = fopen(argv[3], "rb");
        if (input_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        }
        /* On ouvre le fichier de sortie en écriture */
        output_file = fopen(output_file_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Erreur d ouverture de fichier\n");
            exit(1);
        } 

        /* On décompresse le fichier d'entrée
           en utilisant l'algorithme de décompression LZW
           et on écrit la version décompressée du fichier dans le fichier de sortie
           en utilisant le nombre de bits de codage spécifié */
        uncompress(input_file,output_file,atoi(chn_argv2));

        /* On ferme les fichiers d'entrée et de sortie */
        fclose(input_file);
        fclose(output_file);
    }

    /* On libère la mémoire allouée pour le nom du fichier de sortie */
    free(output_file_name);

    return 1;
}
