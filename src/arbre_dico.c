#include "../include/arbre_dico.h"
#include <stdlib.h>
#include <string.h>
#include "../include/LZW.h"

struct Trie * dictionaryInitialization(){
    struct Trie* racine = leafCreation(-1);
    uint32_t * chn_dico;
    int i;

    /* Alloue la mémoire pour chn_dico */
    chn_dico = (uint32_t*)malloc(2*sizeof(uint32_t));
    memset(chn_dico,0,2*sizeof(uint32_t));

    /* Initialise le dictionnaire avec les codes ascii */
    for (i = 0; i < CLEAR_CODE; i++) {
        chn_dico[0] = i;
        insert(racine, chn_dico ,i, 1);
    }  

    /* Libère la mémoire allouée à chn_dico */
    free(chn_dico);
    
    /* Retourne la racine du dictionnaire */
    return racine;
}

struct Trie* leafCreation(int code)
{
    int i;

    /* Alloue la mémoire pour la feuille */
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    /* Initialise le code de la feuille */
    node->code_ascii = code;

    /* Initialise tous les caractères de la feuille à NULL */
    for (i = 0; i < CHAR_SIZE; i++) {
        node->character[i] = NULL;
    }

    /* Retourne la feuille */
    return node;
}
 
void insert(struct Trie *racine, uint32_t * str, int code, int taille)
{
    struct Trie* curr = racine;
    int i=0;
    int compteur = taille;

    /* tant que la fin de la chaîne de caractères n'est pas atteinte */
    while (compteur  > 0)
    {
        /* crée un nouveau nœud si le chemin n'existe pas */
        if (curr->character[str[i]] == NULL) 
            curr->character[str[i]] = leafCreation(code);
 
        /* passe au nœud suivant */
        curr = curr->character[str[i]];
 
        /* passe au caractère suivant */
        i++;
        compteur--;
    }
 
}

int findCodeTree(struct Trie* racine, uint32_t* str, int taille)
{
    int i=0;
    int compteur = taille;
    struct Trie* curr = racine;
    
    /* renvoie -1 si la racine est NULL */
    if (racine == NULL) {
        return -1;
    }
 
    /* tant que la fin de la chaîne de caractères n'est pas atteinte */
    while (compteur>0)
    {
        /* passe au nœud suivant */
        curr = curr->character[str[i]];
    
        /* renvoie -1 si le chemin n'existe pas */
        if (curr == NULL) {
            return -1;
        }
 
        /* passe au caractère suivant */
        i++;
        compteur--;
    }
 
    /* retourne le code associé à la chaîne de caractères */
    return curr->code_ascii;
}
 
void freeTree(struct Trie* racine)
{
    int i;
    /* libère la mémoire allouée pour toutes les feuilles de la racine */
    for (i = 0; i < CHAR_SIZE; i++) {
        if (racine->character[i] != NULL)
            freeTree(racine->character[i]);
    }

    /* libère la mémoire allouée pour la racine */
    if (racine != NULL) {
        free(racine);
    }
  
}
