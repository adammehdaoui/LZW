#include <stdio.h>
#include <stdint.h>

#define CHAR_SIZE 512

struct Trie
{
    int EstFeuille;             
    struct Trie* character[CHAR_SIZE];
    int code_ascii;
    uint32_t caractere[24];
};
 

int quelle_taille(uint32_t * chaine) ;

int compare(uint32_t * x,uint32_t * y);

struct Trie* CreationFeuille(int code);
 
void insert(struct Trie *head, uint32_t * str, int code);
 
int Recherche(struct Trie* head, uint32_t* str);

int A_un_enfant(struct Trie* curr);

int Recherche_code_dans_l_arbre(struct Trie* head, uint32_t* str);

int Recherche_un_noeud(struct Trie* head, int code, uint32_t* str);

int CompteurNoeuds(struct Trie* head);

void Liberation(struct Trie* head);
