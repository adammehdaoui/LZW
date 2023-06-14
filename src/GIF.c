#include "../include/GIF.h"
#include "../include/bit_io.h"
#include "../include/arbre_dico.h"
#include "../include/LZW.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int compressGIF(BIT_FILE * bf, FILE * input_file, int max_code_size, uint8_t * palette, struct Trie* racine, int current_code_size) {
    int next_code = NEXT_CODE;
    uint8_t input_symbol;
    uint32_t * chn;
    uint32_t * last_valid;
    int i, j;
    int taille_last_valid=0;
    int taille_chn=0;
    int last_code = 0;
    int nb_bytes = 0;
    uint8_t input_symbol1;
    int tab_input[8];

    // Alloue la mémoire et initialise chn et last_valid
    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    // Initialise last_code
    last_code = 0; 

    // Ecrit le code CLEAR_CODE dans le fichier de sortie
    bit_put(bf, CLEAR_CODE, current_code_size); 
    nb_bytes += current_code_size;
     
    // tant que le fichier d'entrée n'est pas terminé 
    // ou que le nombre de bytes écrits dans le fichier de sortie 
    // n'est pas supérieur à MAX_ENCOD_GIF (255)
    while ( ! feof(input_file) && nb_bytes/8 < MAX_ENCOD_GIF) { 
        input_symbol1 = getc(input_file);
        // si le caractère lu est NBSP_CODE(255) et que le fichier d'entrée est terminé, on sort de la boucle
        if (input_symbol1 == NBSP_CODE && feof(input_file)) break;

        memset(tab_input,0,8*sizeof(int));
        
        i = 0;
        while (input_symbol1 > 0) {
            tab_input[i] = input_symbol1 % 2;
            input_symbol1 = input_symbol1 / 2;
            i++;
        }
         
        /* Pou le test nous ne considérons que les BMP monochromes*/
        /* le bit 1 correspond à du blanc donc 4 dans la palette */
        /* le bit 0 correspond à du noir donc 0 dans la palette */
        for (i=0;i<8;i++) {
            if (tab_input[i] == 1) input_symbol = 4; //blanc
            else input_symbol = 0; //noir
          
            // chn = last_valid + input_symbol
            j=0;
            while(j<taille_last_valid)
            {
                chn[j] = last_valid[j] ;
                j++;  
            }
            chn[j] = input_symbol;
            j++;
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
                nb_bytes += current_code_size;
                
                // Si le dictionnaire n'est pas plein au regard du nombre de bits de codage courant
                // Alors on ajoute chn au dictionnaire
                if (! (next_code == pow(2,current_code_size) && current_code_size == max_code_size)) {
                    insert(racine, chn ,next_code, taille_chn);
                    next_code++;
                    if (next_code == 255) next_code++;
                    
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
                    nb_bytes += current_code_size;
                    
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
    }
    
    // Ecriture de last_code et END_CODE (257) dans le fichier de sortie
    bit_put(bf, last_code, current_code_size); 
    bit_put(bf, END_CODE, current_code_size); 

    //Liberation de l'arbre et des chaines
    free(chn);
    free(last_valid);

    return(current_code_size);
}

int amountCompressGIF(FILE * input_file, int max_code_size, uint8_t * palette) {
    int next_code = NEXT_CODE;
    uint8_t input_symbol;
    uint32_t * chn;
    uint32_t * last_valid;
    int i, j;
    int taille_last_valid=0;
    int taille_chn=0;
    int current_code_size = MIN_CODE_SIZE;
    struct Trie* racine = dictionaryInitialization();
    int amount_bytes = 0;
    uint8_t input_symbol1;
    int tab_input[8];
    
    // Alloue la mémoire et initialise chn et last_valid
    chn = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    last_valid = (uint32_t*)malloc(TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    memset(last_valid,0,TAILLE_CHN_TEMP*sizeof(uint32_t));

    amount_bytes += current_code_size;
     
    // tant que le fichier d'entrée n'est pas terminé
    while ( ! feof(input_file)) { 
        input_symbol1 = getc(input_file);
        
        memset(tab_input,0,8*sizeof(int));
        // si le caractère lu est NBSP_CODE(255) et que le fichier d'entrée est terminé, on sort de la boucle
        if (input_symbol1 == NBSP_CODE && feof(input_file)) break;

        i = 0;
        while (input_symbol1 > 0) {
            tab_input[i] = input_symbol1 % 2;
            input_symbol1 = input_symbol1 / 2;
        }
        
        for (i=0;i<8;i++) {
            input_symbol = tab_input[i];
            
            // chn = last_valid + input_symbol
            j=0;
            while(j<taille_last_valid)
            {
                chn[j] = last_valid[j] ;
                j++;  
            }
            chn[j] = input_symbol;
            j++;
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
    }    
    
    amount_bytes += current_code_size;
    amount_bytes += current_code_size;

    //Liberation de l'arbre et des chaines
    freeTree(racine);
    free(chn);
    free(last_valid);

    // on a compté les bits mais ce sont les octets que l'on veut
    return amount_bytes/8;
}


void createGIF(FILE * input, FILE * output, int max_code_size)
{
    int i;
    int w, h ;
    BIT_FILE *bf = bit_begin(output);
    uint8_t palette[15];
    int amount_bytes;
    struct Trie* racine; 
    int current_code_size = MIN_CODE_SIZE;

    /* Recherche de la hauteur et largeur du fichier BMP*/ 
    for (i=0; i<18;i++)
        getc(input);
    h = getc(input); // hauteur
    getc(input);    
    getc(input);    
    getc(input);    
    w = getc(input); // largeur 

    /* Retour au début du fichier */  
    fseek(input,0,SEEK_SET);

    /* On commence à générer le GIF */
    /* On écrit dans la zone Header (0 à 5) */
    bit_put(bf,'G',8);
    bit_put(bf,'I',8);
    bit_put(bf,'F',8);
    bit_put(bf,'8',8);
    bit_put(bf,'9',8);
    bit_put(bf,'a',8);

    // On écrit dans la zone Logical Screen Width (6)
    bit_put(bf,w,16);
    // On écrit dans la zone Logical Screen Height (8)
    bit_put(bf,h,16);

    // On écrit dans la zone Global Color Table GCT Flag (10)
    bit_put(bf,247,8); // 	GCT follows for 256 colors with resolution 3 × 8 bits/primary, the lowest 3 bits represent the bit depth minus 1, the highest true bit means that the GCT is present

    // On écrit dans la zone Background Color Index (11)
    bit_put(bf,0,8); // Background color: index #0; #000000 black

    // On écrit dans la zone Default Pixel Aspect Ratio (12)
    bit_put(bf,0,8);

    // On écrit dans la zone Global Color Table (13 à 28 mais 781 possible)
    // pour le test, on écrit une palette de 5 couleurs: noir rouge vert bleu blanc
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
    palette[12] = 255;/* 0 -> white */
    palette[13] = 255;/* 0 -> white */
    palette[14] = 255;/* 0 -> white */

    /* On écrit la palette de couleurs*/
    for (i=0; i<15;i++)
        bit_put(bf,palette[i],8);

    /* On finit de remplir la zone des couleurs*/
    for (i=0; i<753;i++)
        bit_put(bf,0,8);

    /* Pour nous repérer, les commentaires ci-dessous sont les zones */
    /* telles que décrites dans WIKI */
    // An Image Descriptor (introduced by 0x2C, an ASCII comma ',')
    bit_put(bf,',',8);
    bit_put(bf,0,16); // North-west corner position of image in logical screen
    bit_put(bf,0,16); // North-west corner position of image in logical screen
    bit_put(bf,w,16); // Image width and height in pixels
    bit_put(bf,h,16); // Image width and height in pixels
    bit_put(bf,0,8); // Local color table bit, 0 means none
    bit_put(bf,MIN_CODE_SIZE,8); // Start of image, LZW minimum code size 9

    /* On se positionne au début de la zone image du BMP*/
    for (i=0; i<57; i++)
        getc(input);
    
    /* On lance la compression mais sans faire d'écriture */
    /* On cherche à savoir le nombre d'octets qui vont être encodés */
    /* car ils doivent être écrits dans le header du GIF */
    amount_bytes = amountCompressGIF(input, 12, palette); // bytes of image data, see field 320
    
    /* On retourne au début du fichier BMP*/
    /* puis au début de la zone image */
    fseek(input,0,SEEK_SET);
    for (i=0; i<57; i++)
        getc(input);   
    
    /* on initialise le dictionnaire */
    /* car il y a autant d'appels à CompressGIF que de blocs de 255 octets*/
    /* l initialisation dans la fonction aurait vidé le dictionnaire à chaque appel */
    racine = dictionaryInitialization(); 

    // si le nombre d'octets est supérieur à 255
    // on doit découper l'image en plusieurs blocs de 255 octets
    // 255 est un caractère séparateur de chaque bloc, sauf pour le dernier bloc
    while (amount_bytes > MAX_ENCOD_GIF) {
        bit_put(bf, MAX_ENCOD_GIF ,current_code_size);
        bit_flush(bf);
        current_code_size=compressGIF(bf, input, 12, palette, racine, current_code_size); // bytes of image data, see field 320
        amount_bytes = amount_bytes - MAX_ENCOD_GIF;
        bit_flush(bf);
    }

    // on traite le dernier bloc */
    bit_put(bf, amount_bytes, 16); // Amount of LZW encoded image follow
    bit_flush(bf);
    compressGIF(bf, input, 12, palette, racine, current_code_size); // bytes of image data, see field 320
    bit_flush(bf);

    bit_put(bf,0,8); // End of image data block
    bit_put(bf,59,8); // Trailer: Last byte in the file, signaling EOF (point virgule)

    bit_end(bf);
    freeTree(racine);
    return;
}

