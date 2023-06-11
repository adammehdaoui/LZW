#include "../include/GIF.h"
#include "../include/bit_io.h"
#include "../include/arbre_dico.h"
#include "../include/LZW.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

void compressGIF(BIT_FILE * bf, FILE * input_file, int max_code_size) {
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

    // Alloue la mémoire et initialise chn et last_valid
    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    // Initialise last_code
    last_code = 0; 

    // Ecrit le code CLEAR_CODE dans le fichier de sortie
    bit_put(bf, CLEAR_CODE, current_code_size); 
    //printf("ECRITURE %u en %d bits\n",256,current_code_size);
     
    // tant que le fichier d'entrée n'est pas terminé
    while ( ! feof(input_file)) { 
        input_symbol = getc(input_file);
        //printf("%d ",input_symbol);

        // si le caractère lu est NBSP_CODE(255) et que le fichier d'entrée est terminé, on sort de la boucle
        if (input_symbol == NBSP_CODE && feof(input_file)) break;
        
        // chn = last_valid + input_symbol
        i=0;
        while(i<taille_last_valid)
        {
            chn[i] = last_valid[i] ;
            i++;  
        }
        chn[i] = input_symbol;
        i++;
        taille_chn = taille_last_valid + 1;    
        
        // Recherche de chn dans le dictionnaire
        if (findCodeTree(racine,chn,taille_chn) != -1) 
        {   // Le code existe dans le dictionnaire

            // last_valid = chn
            memcpy(last_valid,chn,TAILLE_CHN_TEMP*sizeof(uint32_t));
            taille_last_valid = taille_chn;

            // last_code recherché dans le dictionnaire
            last_code = findCodeTree(racine, chn, taille_chn);
        } 
        else  
        {   // Le code n existe pas: ajout au dictionnaire

            // Ecriture de last_code dans le fichier de sortie
            bit_put(bf, last_code, current_code_size); 
            
            // Si le dictionnaire n'est pas plein au regard du nombre de bits de codage courant
            // Alors on ajoute chn au dictionnaire
            if (! (next_code == pow(2,current_code_size) && current_code_size == max_code_size)) {
                insert(racine, chn ,next_code, taille_chn);
                next_code++;
                
                // on ne peut plus ajouter de code dans le dictionnaire
                // alors il faut augmenter le nombre de bits de codage
                if (next_code == pow(2,current_code_size)+1) {
                    current_code_size++;
                }    
            }
            // Sinon on réinitialise le dictionnaire
            // on réinitialise les variables associées, 
            // le nombre de bits de codage courant et le prochain code à ajouter
            else
            {
                // Ecriture de CLEAR_CODE dans le fichier de sortie
                bit_put(bf, CLEAR_CODE, current_code_size);
                
                // Réinitialisation des variables
                current_code_size = MIN_CODE_SIZE;
                next_code = NEXT_CODE;
                
                // Réinitialisation du dictionnaire
                freeTree(racine);
                racine = dictionaryInitialization();
            }

            // last_valid = input_symbol
            last_valid[0] = input_symbol;
            taille_last_valid = 1;

            // last_code = input_symbol
            last_code = input_symbol;
        }
      
    }
    
    // Ecriture de last_code et END_CODE (257) dans le fichier de sortie
    bit_put(bf, last_code, current_code_size); 
    bit_put(bf, END_CODE, current_code_size); 

    //Liberation de l'arbre et des chaines
    freeTree(racine);
    free(chn);
    free(last_valid);
}

int amountCompressGIF(FILE * input_file, int max_code_size) {
    int next_code = NEXT_CODE;
    uint8_t input_symbol;
    uint32_t * chn;
    uint32_t * last_valid;
    int i;
    int taille_last_valid=0;
    int taille_chn=0;
    int current_code_size = MIN_CODE_SIZE;
    struct Trie* racine = dictionaryInitialization();
    int amount_bytes = 0;

    // Alloue la mémoire et initialise chn et last_valid
    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    amount_bytes += current_code_size;
     
    // tant que le fichier d'entrée n'est pas terminé
    while ( ! feof(input_file)) { 
        input_symbol = getc(input_file);
        //printf("%d ",input_symbol);

        // si le caractère lu est NBSP_CODE(255) et que le fichier d'entrée est terminé, on sort de la boucle
        if (input_symbol == NBSP_CODE && feof(input_file)) break;
        
        // chn = last_valid + input_symbol
        i=0;
        while(i<taille_last_valid)
        {
            chn[i] = last_valid[i] ;
            i++;  
        }
        chn[i] = input_symbol;
        i++;
        taille_chn = taille_last_valid + 1;    
        
        // Recherche de chn dans le dictionnaire
        if (findCodeTree(racine,chn,taille_chn) != -1) 
        {   // Le code existe dans le dictionnaire

            // last_valid = chn
            memcpy(last_valid,chn,TAILLE_CHN_TEMP*sizeof(uint32_t));
            taille_last_valid = taille_chn;
        } 
        else  
        {   // Le code n existe pas: ajout au dictionnaire

            // Ecriture de last_code dans le fichier de sortie
            amount_bytes += current_code_size;
            
            // Si le dictionnaire n'est pas plein au regard du nombre de bits de codage courant
            // Alors on ajoute chn au dictionnaire
            if (! (next_code == pow(2,current_code_size) && current_code_size == max_code_size)) {
                insert(racine, chn ,next_code, taille_chn);
                next_code++;
                
                // on ne peut plus ajouter de code dans le dictionnaire
                // alors il faut augmenter le nombre de bits de codage
                if (next_code == pow(2,current_code_size)+1) {
                    current_code_size++;
                }    
            }
            // Sinon on réinitialise le dictionnaire
            // on réinitialise les variables associées, 
            // le nombre de bits de codage courant et le prochain code à ajouter
            else
            {
                amount_bytes += current_code_size;
                
                // Réinitialisation des variables
                current_code_size = MIN_CODE_SIZE;
                next_code = NEXT_CODE;
                
                // Réinitialisation du dictionnaire
                freeTree(racine);
                racine = dictionaryInitialization();
            }

            // last_valid = input_symbol
            last_valid[0] = input_symbol;
            taille_last_valid = 1;
        }
    }
    
    amount_bytes += current_code_size;
    amount_bytes += current_code_size;

    //Liberation de l'arbre et des chaines
    freeTree(racine);
    free(chn);
    free(last_valid);

    return amount_bytes;
}


void createGIF(FILE * input, FILE * output, int max_code_size)
{
    int i;
    int w = 96, h = 96;
    BIT_FILE *bf = bit_begin(output);
    uint8_t palette[15];
    int amount_bytes;

    // Zone Header (0 à 5)
    bit_put(bf,'G',8);
    bit_put(bf,'I',8);
    bit_put(bf,'F',8);
    bit_put(bf,'8',8);
    bit_put(bf,'9',8);
    bit_put(bf,'a',8);

    // Zone Logical Screen Width (6)
    bit_put(bf,w,16);
    // Zone Logical Screen Height (8)
    bit_put(bf,h,16);

    // Zone Global Color Table GCT Flag (10)
    bit_put(bf,241,8); // 	GCT follows for 256 colors with resolution 3 × 8 bits/primary, the lowest 3 bits represent the bit depth minus 1, the highest true bit means that the GCT is present

    // Zone Background Color Index (11)
    bit_put(bf,0,8); // Background color: index #0; #000000 black

    // Zone Default Pixel Aspect Ratio (12)
    bit_put(bf,0,8);

    // Zone Global Color Table (13 à 28 mais 781 possible)
    palette[0] = 0;/* 0 -> black */
    palette[1] = 0;/* 0 -> black */
    palette[2] = 0;/* 0 -> black */
    palette[3] = 255;/* 0 -> red */
    palette[4] = 0;/* 0 -> red */
    palette[5] = 0;/* 0 -> red */
    palette[6] = 0;/* 0 -> green */
    palette[7] = 255;/* 0 -> green */
    palette[8] = 0;/* 0 -> green */
    palette[9] = 0;/* 0 -> blue */
    palette[10] = 0;/* 0 -> blue */
    palette[11] = 255;/* 0 -> blue */    
    palette[12] = 0;/* 0 -> white */
    palette[13] = 0;/* 0 -> white */
    palette[14] = 255;/* 0 -> white */
    for (i=0; i<15;i++)
        bit_put(bf,palette[i],8);

    // An Image Descriptor (introduced by 0x2C, an ASCII comma ',')
    bit_put(bf,',',8);
    bit_put(bf,0,16); //North-west corner position of image in logical screen
    bit_put(bf,0,16); //North-west corner position of image in logical screen
    bit_put(bf,w,16); //	Image width and height in pixels
    bit_put(bf,h,16); // 	Image width and height in pixels
    bit_put(bf,0,8); // Local color table bit, 0 means none
    bit_put(bf,9,8); // Start of image, LZW minimum code size 9

    amount_bytes = amountCompressGIF(input, 12); // bytes of image data, see field 320
    bit_put(bf, 10+(amount_bytes/8), 8); // Amount of LZW encoded image follow, 11 bytes
    fseek(input,0,SEEK_SET);
    compressGIF(bf, input, 12); // bytes of image data, see field 320

    bit_flush(bf);

    bit_put(bf,0,8); // End of image data block
    bit_put(bf,59,8); // Trailer: Last byte in the file, signaling EOF

    bit_end(bf);
    return;
}

