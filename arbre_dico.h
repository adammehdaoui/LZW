#include <stdio.h>
#include <stdint.h>

#define CHAR_SIZE 512

struct Trie
{
    struct Trie* character[CHAR_SIZE];
    int code_ascii;
};
 

int quelle_taille(uint32_t * chaine) ;

struct Trie* CreationFeuille(int code);
 
void insert(struct Trie *head, uint32_t * str, int code);
 
int Recherche(struct Trie* head, uint32_t* str);

int A_un_enfant(struct Trie* curr);

int Recherche_code_dans_l_arbre(struct Trie* head, uint32_t* str);

int Recherche_un_noeud(struct Trie* head, int code, uint32_t* str);

int CompteurNoeuds(struct Trie* head);

void Liberation(struct Trie* head);

struct Trie* Trouver_Noeud_Pere(struct Trie* head, struct Trie* child);

int Lien_character_pere_fils(struct Trie* pere, struct Trie* fils);

int CompteurNiveaux(struct Trie * head,struct Trie * node);