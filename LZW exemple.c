#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit_io.h"

#define DICTIONARY_SIZE 4096
#define CLEAR_CODE 256
#define END_CODE 257
#define MAX_CODE_SIZE 12

#define BITS 9

void compress(FILE *input_file,FILE *output_file) {

    uint16_t next_code = 258;
    uint8_t input_symbol;
    char str[12];
    char chn[24],chn_sauv[24],last_valid[1000];
    int i;
    int Trouve;
    int last_code = 0;
    char dictionary[512][12];

    BIT_FILE *bf = bit_begin(output_file);

   // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        dictionary[i][0] =(char)i;
        dictionary[i][1] = '\0';
    }    
    
    for (i = 256; i < 512; i++) {
        dictionary[i][0] = '\0';
    }        
    //fprintf(output_file,"%s",bin(256));
    bit_put(bf, 256, 9);
    fprintf(stderr,"Envoyé %d\n",256);
    strcpy(last_valid,"");
    
    while ( ! feof(input_file)) { //while not end of file do
        input_symbol = getc(input_file);
        if(input_symbol == 255) break;
        //Conversion en chaines de caractères
        str[0] = input_symbol;
        str[1] = '\0';
        strcpy(chn, last_valid);
        strcat(chn, str);
        
        Trouve = 0;
        for (i=0;i<512;i++){
            if (strcmp(chn, dictionary[i]) == 0){
                Trouve = i;
                break;
            }
        }

        if (Trouve != 0) 
        {
            // Le code existe dans le dictionnaire
            strcpy(last_valid,chn);
            last_code = Trouve;
            //printf("%s Trouvé! Last_valid %s  Last_code %d  \n",chn,last_valid,last_code);
        } 
        else 
        {
            // Code does not exist in the dictionary
            strcat(dictionary[next_code],chn);
            //fprintf(stderr,"Ajout dico dic:%s next:%d chn:%s\n",dictionary[next_code],next_code,chn);
            next_code++;

            strcpy(chn_sauv,chn);
            if (last_code != 0) {
                fprintf(stderr,"Envoyé %d\n",last_code);
                bit_put(bf, last_code, 9);  
            }

            str[0] = input_symbol;
            str[1] = '\0';
            strcpy(last_valid, str);
            strcpy(chn,chn_sauv);

            for (i=0;i<512;i++){
                if (strcmp(last_valid, dictionary[i]) == 0){
                    last_code = i;
                    break;
                }
            }   
            //printf("%s Pas trouvé: Last valid %s  Last code %d  Next code %d\n",chn,last_valid,last_code,next_code);
        }
    }

    fprintf(stderr,"Envoyé %d\n", last_code);   
    bit_put(bf, last_code, 9);
    //fprintf(output_file," %s", bin(END_CODE));
    bit_put(bf, END_CODE, 9);

    bit_flush(bf);
    bit_end(bf);

}

void decompress(FILE *input_file,FILE *output_file) {
    uint16_t next_code = 258;
    uint32_t code;
    uint32_t prev_code;
    uint16_t current_code_size = 9;
    uint16_t max_code_size = MAX_CODE_SIZE;
    char dictionary[512][12];
    int i;
    int   currentNumber;
    char   *endChar;
    char  buffer[1000];
    char chn[12];
    char last_valid[12];
    char seq[12];
    char str[12];

    BIT_FILE *bf = bit_begin(input_file);

    // Initialize dictionary with single-character codes
    for (i = 0; i < 256; i++) {
        dictionary[i][0] =(char)i;
        dictionary[i][1] = '\0';
    }     
    for (i = 256; i < 512; i++) {
        dictionary[i][0] = '\0';
    }    

    strcpy(last_valid,"");
    printf("---------------\n");

   /*une boucle qui prend chaque ligne les unes apres les autres jusqu'à la fin du fichier*/
    while (bit_get(bf, &code, BITS) != EOF) {
            currentNumber = code;

            if (currentNumber != 257 && currentNumber != 256)
            {
                sprintf(chn,"%d",currentNumber);
                printf("Analyse du %d\n",currentNumber);
            
                if (strcmp(dictionary[currentNumber],"") != 0){ //Trouvé
                    strcpy(seq,dictionary[currentNumber]);
                    if (strcmp(last_valid,"") != 0) {
                        strcpy(dictionary[next_code],last_valid);
                        str[0] = seq[0];
                        str[1] = '\0';
                        strcat(dictionary[next_code],str);
                        //printf("Ajout dico dic:%s next:%d last:%s str:%s\n",dictionary[next_code],next_code,last_valid,str);
                        next_code++;
                    }
                    
                    strcpy(last_valid,dictionary[currentNumber]);
                    fprintf(output_file,"%s",dictionary[currentNumber]);
                    //printf("%d Trouvé! last:%s seq:%s next:%d\n",currentNumber,last_valid,seq,next_code);
                } 
                else //Pas trouvé
                {
                    str[0] = last_valid[0];
                    str[1] = '\0';
                    strcpy(seq,last_valid);
                    strcat(seq,str);

                    strcpy(dictionary[next_code],last_valid);
                    str[0] = seq[0];
                    str[1] = '\0';
                    strcat(dictionary[next_code],str);
                    //printf("Ajout dico dic:%s next:%d last:%s str:%s\n",dictionary[next_code],next_code,last_valid,str);
                    next_code++;

                    strcpy(last_valid,seq);
                    fprintf(output_file,"%s",seq);
                    //printf("%d Pas Trouvé! last:%s seq:%s next:%d\n",currentNumber,last_valid,seq,next_code);
                }
            }     
    }

    bit_flush(bf);
    bit_end(bf);

}

int main() {
    FILE *input_file = fopen("comp_input.txt", "rb");
    FILE *output_file = fopen("comp_output.txt", "w");
    compress(input_file,output_file);
    fclose(input_file);    
    fclose(output_file);

    input_file = fopen("comp_output.txt", "r");
    output_file = fopen("decomp_output.txt", "wb");
    decompress(input_file,output_file);
    fclose(input_file); 
    fclose(output_file);
}

