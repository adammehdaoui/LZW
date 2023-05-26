#include "arbre_dico.h"
#include <stdlib.h>
#include <string.h>

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
void insert(struct Trie *head, uint16_t * str, int code)
{
    // partir du nœud racine
    struct Trie* curr = head;
    int i=0;

    while (str[i])
    {
        //printf("dans le while %u %d\n",str[i],i);
        // crée un nouveau nœud si le chemin n'existe pas
        if (curr->character[str[i]] == NULL) 
            curr->character[str[i]] = getNewTrieNode(code);
 
        // passe au nœud suivant
        curr = curr->character[str[i]];
 
        // passe au caractère suivant
        i++;
    }
 
    // marque le nœud courant comme une feuille
    curr->isLeaf = 1;
    memcpy(curr->caractere,str,quelle_taille(str)*sizeof(uint16_t));
}
 
// Fonction itérative pour rechercher une string dans un Trie. Il renvoie 1
// si la string est trouvée dans le Trie ; sinon, il renvoie 0.
int search(struct Trie* head, uint16_t * str)
{
    int i=0;
    
    // retourne 0 si Trie est vide
    if (head == NULL) {
        return 0;
    }
 
    struct Trie* curr = head;
    while (str[i])
    {
        // passe au nœud suivant
        curr = curr->character[str[i]];
 
        // si la string est invalide (a atteint la fin d'un chemin dans le Trie)
        if (curr == NULL) {
            return 0;
        }
 
        // passe au caractère suivant
        i++;
    }
 
    // renvoie 1 si le noeud courant est une feuille et que le
    // la fin de la string est atteinte
    return curr->isLeaf;
}


 int search_code(struct Trie* head, uint16_t* str)
{
    int i=0;
    
    // retourne 0 si Trie est vide
    if (head == NULL) {
        return -1;
    }
 
    struct Trie* curr = head;
    while (str[i])
    {
        // passe au nœud suivant
        curr = curr->character[str[i]];
 
        // si la string est invalide (a atteint la fin d'un chemin dans le Trie)
        if (curr == NULL) {
            return -1;
        }
 
        // passe au caractère suivant
        i++;
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
int deletion(struct Trie **curr, uint16_t * str)
{
    int i=0;
    
    // retourne 0 si Trie est vide
    if (*curr == NULL) {
        return 0;
    }
 
    // si la fin de la string n'est pas atteinte
    if (str[i])
    {
        // se répète pour le nœud correspondant au caractère suivant dans
        // la string et si elle renvoie 1, supprime le nœud courant
        // (si ce n'est pas une feuille)
        if (*curr != NULL && (*curr)->character[str[i]] != NULL &&
            deletion(&((*curr)->character[str[i]]), str + 1) &&
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
 

// Fonction récursive pour rechercher un code ASCII dans un Trie et renvoyer la chaîne correspondante
struct Trie* searchHelper(struct Trie* node, int code)
{
    if (node == NULL) {
        return NULL;
    }

    // Si le nœud courant est une feuille et son code correspond au code recherché
    if (node->isLeaf && node->code_ascii == code)
    {
        return node;
    }

    // Parcourt les caractères suivants dans le Trie
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (node->character[i] != NULL)
        {
            // Appelle récursivement la recherche pour les nœuds enfants
            struct Trie* result = searchHelper(node->character[i], code);
            if (result != NULL) {
                return result;
            }
        }
    }
    //printf("pas trouvé\n");
    return NULL;
}

// Fonction pour rechercher un code ASCII dans un Trie et renvoyer la chaîne correspondante
int search_node(struct Trie* head, int code, uint16_t* str)
{
    struct Trie* node;
    
    if (head == NULL ) {
        return 0;
    }    
    //printf("code = %d\n",code);
    node = searchHelper(head, code);
    if (node != NULL) {
        //printf("node = %p car %u %u %u\n",node,node->caractere[0],node->caractere[1],node->caractere[2]);
        memcpy(str,node->caractere,quelle_taille(node->caractere)*sizeof(uint16_t));
        return 1;
    }
    
    if (node == NULL) {
        return 0;
    }
    return 1;
}
