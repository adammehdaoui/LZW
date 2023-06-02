#include <stdio.h>
#include <stdint.h>

#define CHAR_SIZE 512

struct Trie
{
    struct Trie* character[CHAR_SIZE];
    int code_ascii;
};
 
struct Trie* CreationFeuille(int code);
 
void insert(struct Trie *racine, uint32_t * str, int code, int taille);
 
/*int Recherche(struct Trie* racine, uint32_t* str);*/

int A_un_enfant(struct Trie* curr);

int Recherche_code_dans_l_arbre(struct Trie* racine, uint32_t* str, int taille);

int Recherche_un_noeud(struct Trie* racine, int code, uint32_t* str);

int CompteurNoeuds(struct Trie* racine);

void Liberation(struct Trie* racine);

struct Trie* Trouver_Noeud_Pere(struct Trie* racine, struct Trie* child);

int Lien_character_pere_fils(struct Trie* pere, struct Trie* fils);

int CompteurNiveaux(struct Trie * racine,struct Trie * node);

struct Trie* Recherche_dans_l_arbre(struct Trie* node, int code);
