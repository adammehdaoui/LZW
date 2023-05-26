#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit_io.h"
#include "arbre_dico.h"

#define CLEAR_CODE 256
#define END_CODE 257
#define NEXT_CODE 258
#define MAX_CODE_SIZE 12

#define BITS 9

void compress(FILE *input_file,FILE *output_file) {
    BIT_FILE *bf = bit_begin(output_file);

    uint16_t next_code = NEXT_CODE;
    uint8_t input_symbol;
    //uint16_t current_code_size = 9;
    uint16_t chn[24];
    uint16_t last_valid[24];
    int i;
    int j;
    int taille;
    uint16_t last_code = 0;

    struct Trie* head = getNewTrieNode(-1);

    for (i=0;i<24;i++)
        {
            chn[i] = 0;
            last_valid[i] = 0;
        }
    // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        chn[0] = i;
        for (j=1;j<12;j++)
            chn[j] = 0;
        insert(head, chn ,i);
    }  
    
    last_code = 0; 
    //fprintf(output_file,"%u ",256);
    bit_put(bf, 256, 9);
    
    while ( ! feof(input_file)) { //while not end of file do
        input_symbol = getc(input_file);
        //printf("input_symbol %d\n",input_symbol);
        if(input_symbol == 255) break;//fin
        //Conversion en chaines de caractères
        i=0;

        taille = quelle_taille(last_valid);
        while(i<taille)
            {
                chn[i] = last_valid[i] ;i++;  
            }
        chn[i] = input_symbol;
        i++;
        while(i<24)
            {
                chn[i] = 0 ;i++;  
            }

        //printf("search de %u %u %u :%d code %d\n",chn[0],chn[1],chn[2],search(head,chn),search_code(head,chn));

        if (search(head,chn) == 1) 
        {
            // Le code existe dans le dictionnaire
            for (i=0; i< 12; i++)
                last_valid[i] = chn[i];
            last_code = search_code(head,chn);
            //printf("%u %u %u Trouvé! Last_valid %u %u %u  Last_code %d  \n",chn[0],chn[1],chn[2],last_valid[0],last_valid[1],last_valid[2],last_code);
        } 
        else 
        {
            // Code does not exist in the dictionary
            //bit_put(bf, prev_code, current_code_size);
            insert(head, chn ,next_code);
            //printf("Ajouté %u %u %u en %d\n",chn[0],chn[1],chn[2],next_code);
            next_code++;

            //fprintf(output_file,"%u ",last_code);
            bit_put(bf, last_code, 9); 

            last_valid[0] = input_symbol;
            last_valid[1] = 0;
            last_valid[2] = 0;

            chn[0] = last_valid[0];
            for (i=1;i<12;i++)
                chn[i] = 0;
            last_code = search_code(head,chn);
            //printf("%u %u %u Pas trouvé: Last valid %u %u %u Last code %d  Next code %d\n",chn[0],chn[1],chn[2],last_valid[0],last_valid[1],last_valid[2],last_code,next_code);
        }
    }
    //fprintf(output_file,"%u ",last_code);
    bit_put(bf, last_code, 9);
    //fprintf(output_file,"%u",END_CODE);
    bit_put(bf, END_CODE, 9);

    // Fin du fichier
    //bit_put(bf, prev_code, current_code_size);
    //bit_put(bf, END_CODE, current_code_size);

    bit_flush(bf);
    bit_end(bf);

    for (i = 0; i < 256; i++) {
        chn[0] = i;
        deletion(&head, chn);
    }

}

void decompress(FILE *input_file,FILE *output_file) {
    BIT_FILE *bf = bit_begin(input_file);

    uint16_t next_code = NEXT_CODE;
    //uint16_t current_code_size = 9;
    //uint16_t max_code_size = MAX_CODE_SIZE;
    int i, j;
    int taille;
    int currentNumber;
    char *endChar;
    char buffer[1000];
    uint16_t last_valid[24];
    uint16_t seq[24];
    uint16_t chn[24];
    int Trouve=0;
    uint32_t code;

    struct Trie* head = getNewTrieNode(-1);

    for (i=0;i<24;i++)
        {
            chn[i] = 0;
            last_valid[i] = 0;
            seq[i] = 0;
        }
    // Initialise le dictionnaire avec les codes ascii
    for (i = 0; i < 256; i++) {
        chn[0] = i;
        for (j=1;j<12;j++)
            chn[j] = 0;
        insert(head, chn ,i);
    }  
    
   //une boucle qui prend chaque ligne les unes apres les autres jusqu'à la fin du fichier
    while (bit_get(bf, &code, BITS) != EOF) {
            currentNumber = code;

            if (currentNumber != 257 && currentNumber != 256)
            {
                printf("Analyse du %d\n",currentNumber);

                Trouve = search_node(head,currentNumber,*(&seq));
                printf("Trouve %d seq %u %u %u\n",Trouve,seq[0],seq[1],seq[2]);

                if (Trouve != 0){ //Trouvé
                    taille = quelle_taille(seq);
                    
                    printf("Seq %u %u %u\n",seq[0],seq[1],seq[2]);
                    if (last_valid[0] != 0) {
                        taille = quelle_taille(last_valid);
                        for (i=0;i<taille;i++)
                            chn[i] = last_valid[i];
                        chn[taille] = seq[0];
                        //printf("Taille %d\n",taille);
                        insert(head,chn,next_code);
                        printf("Ajout dico en:%d last:%u %u %u\n",next_code,chn[0],chn[1],chn[2]);
                        next_code++;
                    }
                    
                    Trouve = search_node(head,currentNumber,&(*last_valid));
                    
                    taille = quelle_taille(seq); 
                    for (i=0;i<taille;i++)
                        fprintf(output_file,"%c", seq[i]);   
                    printf("%d Trouvé! last:%u %u %u seq:%u %u %u next:%d\n",currentNumber,last_valid[0],last_valid[1],last_valid[2],seq[0],seq[1],seq[2],next_code);
                } 
                else //Pas trouvé
                {
                    taille = quelle_taille(last_valid);
                    for (i=0;i<taille;i++)
                        seq[i] = last_valid[i];
                    seq[taille] = last_valid[0];

                    taille = quelle_taille(last_valid);
                        for (i=0;i<taille;i++)
                            chn[i] = last_valid[i];
                        chn[taille] = seq[0];
                    insert(head,chn,next_code);
                    printf("Ajout dico next:%d last:%u %u %u\n",next_code,chn[0],chn[1],chn[2]);
                    next_code++;

                    taille = quelle_taille(seq);
                    for (i=0;i<taille;i++)
                        last_valid[i] = seq[i];

                    taille = quelle_taille(seq); 
                    for (i=0;i<taille;i++)
                        fprintf(output_file,"%c", seq[i]);     
                    printf("%d Pas Trouvé! last:%u %u %u seq:%u %u %u next:%d\n",currentNumber,last_valid[0],last_valid[1],last_valid[2],seq[0],seq[1],seq[2],next_code);
                }
            }       
    }

    bit_flush(bf);
    bit_end(bf);
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



// ETrie l'implémentation en C - Insertion, Recherche et Suppression
/*int main()
{

    insert(head, "A" ,65);
    printf("%d ", search(head, "A"));       // imprimer 1
 
    insert(head, "AB", 66);
    printf("%d ", search(head, "AB"));  // imprimer 1
 
    printf("%d ", search(head, "C"));       // affiche 0 (absent)
 
    insert(head, "hell",67);
    printf("%d ", search(head, "hell"));        // imprimer 1
 
    insert(head, "h", 68);
    printf("%d \n", search(head, "h"));         // imprimer 1 + retour à la ligne
 
    deletion(&head, "hello");
    printf("%d ", search(head, "hello"));       // print 0 (hello deleted)
    printf("%d ", search(head, "helloworld"));  // imprimer 1
    printf("%d \n", search(head, "hell"));      // imprimer 1 + retour à la ligne
 
    deletion(&head, "h");
    printf("%d ", search(head, "h"));           // affiche 0 (h supprimé)
    printf("%d ", search(head, "hell"));        // imprimer 1
    printf("%d\n", search(head, "helloworld")); // imprimer 1 + retour à la ligne
 
    deletion(&head, "helloworld");
    printf("%d ", search(head, "helloworld"));  // imprime 0
    printf("%d ", search(head, "hell"));        // imprimer 1
 
    deletion(&head, "hell");
    printf("%d\n", search(head, "hell"));       // affiche 0 + retour à la ligne
 
    if (head == NULL) {
        printf("Trie empty!!\n");               // Trie est vide maintenant
    }
 
    printf("%d ", search(head, "hell"));        // imprime 0
 
    return 0;
}
*/