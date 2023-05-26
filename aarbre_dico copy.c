#include "arbre_dico.h"
#include <stdlib.h>

#define CHAR_SIZE 26
 

int quelle_taille(uint16_t * chaine){
    int i=0;
    while(chaine[i] != 0){
        i++;
    }
    return i;
}

int compare(uint16_t * x,uint16_t * y){
    int i=0;
    int taillex = quelle_taille(x);
    int tailley = quelle_taille(y);

    while(i<taillex && i<tailley){
        if (x[i] > y[i]) return 1;
        if (x[i] < y[i]) return -1;
        i++;
    }

    if (taillex == tailley) return 0;
    else return 1;
}


// Fonction qui renvoie un nouveau noeud Trie
struct Trie* getNewTrieNode(int code)
{
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    node->isLeaf = 0;
    node->code_ascii = code;
 
    for (int i = 0; i < CHAR_SIZE; i++) {
        node->character[i] = NULL;
    }
 
    return node;
}
 
// Fonction itérative pour insérer une string dans un Trie
void insert(struct Trie *head, char* str, int code)
{
    // partir du nœud racine
    struct Trie* curr = head;
    while (*str)
    {
        // crée un nouveau nœud si le chemin n'existe pas
        if (curr->character[*str - 'a'] == NULL) {
            curr->character[*str - 'a'] = getNewTrieNode(code);
        }
 
        // passe au nœud suivant
        curr = curr->character[*str - 'a'];
 
        // passe au caractère suivant
        str++;
    }
 
    // marque le nœud courant comme une feuille
    curr->isLeaf = 1;
}
 
// Fonction itérative pour rechercher une string dans un Trie. Il renvoie 1
// si la string est trouvée dans le Trie ; sinon, il renvoie 0.
int search(struct Trie* head, char* str)
{
    // retourne 0 si Trie est vide
    if (head == NULL) {
        return 0;
    }
 
    struct Trie* curr = head;
    while (*str)
    {
        // passe au nœud suivant
        curr = curr->character[*str - 'a'];
 
        // si la string est invalide (a atteint la fin d'un chemin dans le Trie)
        if (curr == NULL) {
            return 0;
        }
 
        // passe au caractère suivant
        str++;
    }
 
    // renvoie 1 si le noeud courant est une feuille et que le
    // la fin de la string est atteinte
    return curr->isLeaf;
}


 int search_code(struct Trie* head, char* str)
{
    // retourne 0 si Trie est vide
    if (head == NULL) {
        return -1;
    }
 
    struct Trie* curr = head;
    while (*str)
    {
        // passe au nœud suivant
        curr = curr->character[*str - 'a'];
 
        // si la string est invalide (a atteint la fin d'un chemin dans le Trie)
        if (curr == NULL) {
            return -1;
        }
 
        // passe au caractère suivant
        str++;
    }
 
    // renvoie 1 si le noeud courant est une feuille et que le
    // la fin de la string est atteinte
    return curr->code_ascii;
}
 
// Renvoie 1 si un nœud Trie donné a des enfants
int hasChildren(struct Trie* curr)
{
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (curr->character[i]) {
            return 1;       // enfant trouvé
        }
    }
 
    return 0;
}
 
// Fonction récursif pour supprimer une string d'un Trie
int deletion(struct Trie **curr, char* str)
{
    // retourne 0 si Trie est vide
    if (*curr == NULL) {
        return 0;
    }
 
    // si la fin de la string n'est pas atteinte
    if (*str)
    {
        // se répète pour le nœud correspondant au caractère suivant dans
        // la string et si elle renvoie 1, supprime le nœud courant
        // (si ce n'est pas une feuille)
        if (*curr != NULL && (*curr)->character[*str - 'a'] != NULL &&
            deletion(&((*curr)->character[*str - 'a']), str + 1) &&
            (*curr)->isLeaf == 0)
        {
            if (!hasChildren(*curr))
            {
                free(*curr);
                (*curr) = NULL;
                return 1;
            }
            else {
                return 0;
            }
        }
    }
 
    // si la fin de la string est atteinte
    if (*str == '\0' && (*curr)->isLeaf)
    {
        // si le nœud courant est un nœud feuille et n'a pas d'enfant
        if (!hasChildren(*curr))
        {
            free(*curr);    // supprimer le noeud courant
            (*curr) = NULL;
            return 1;       // supprimer les nœuds parents non feuilles
        }
 
        // si le nœud courant est un nœud feuille et a des enfants
        else {
            // marque le nœud actuel comme un nœud non-feuille (NE LE SUPPRIMEZ PAS)
            (*curr)->isLeaf = 0;
            return 0;       // ne supprime pas ses nœuds parents
        }
    }
 
    return 0;
}
 