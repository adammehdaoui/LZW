#include "arbre_dico.h"
#include <stdlib.h>
#include <string.h>

int quelle_taille(uint32_t * chaine){
    int i=0;
    while(chaine[i] != 0){
        i++;
    }
    return i;
}

int compare(uint32_t * x,uint32_t * y){
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
struct Trie* CreationFeuille(int code)
{
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    //printf("creation noeud %p  %d\n",node,code);
    node->EstFeuille = 0;
    node->code_ascii = code;
    
    for (int i = 0; i < CHAR_SIZE; i++) {
        node->character[i] = NULL;
    }

    memset(node->caractere,0,24*sizeof(uint32_t));
    
    return node;
}
 
// Fonction itérative pour insérer une string dans un Trie
void insert(struct Trie *head, uint32_t * str, int code)
{
    // partir du nœud racine
    struct Trie* curr = head;
    int i=0;
    //printf("creation noeud %u %u %u %u %u %u %u\n",str[0],str[1],str[2],str[3],str[4],str[5],str[6]);

    while (str[i])
    {
        //printf("dans le while %u %d\n",str[i],i);
        // crée un nouveau nœud si le chemin n'existe pas
        if (curr->character[str[i]] == NULL) 
            curr->character[str[i]] = CreationFeuille(code);
 
        // passe au nœud suivant
        curr = curr->character[str[i]];
 
        // passe au caractère suivant
        i++;
    }
 
    // marque le nœud courant comme une feuille
    curr->EstFeuille = 1;
    memcpy(curr->caractere,str,quelle_taille(str)*sizeof(uint32_t));
}
 
// Fonction itérative pour rechercher une string dans un Trie. Il renvoie 1
// si la string est trouvée dans le Trie ; sinon, il renvoie 0.
int Recherche(struct Trie* head, uint32_t * str)
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
    return curr->EstFeuille;
}


 int Recherche_code_dans_l_arbre(struct Trie* head, uint32_t* str)
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
int A_un_enfant(struct Trie* curr)
{
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (curr->character[i]) {
            return 1;       // enfant trouvé
        }
    }
 
    return 0;
}

// Fonction récursive pour rechercher un code ASCII dans un Trie et renvoyer la chaîne correspondante
struct Trie* Recherche_dans_l_arbre(struct Trie* node, int code)
{
    if (node == NULL) {
        return NULL;
    }

    // Si le nœud courant est une feuille et son code correspond au code recherché
    if (node->EstFeuille && node->code_ascii == code)
    {
        return node;
    }

    // Parcourt les caractères suivants dans le Trie
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (node->character[i] != NULL)
        {
            // Appelle récursivement la recherche pour les nœuds enfants
            struct Trie* result = Recherche_dans_l_arbre(node->character[i], code);
            if (result != NULL) {
                return result;
            }
        }
    }
    //printf("pas trouvé\n");
    return NULL;
}

// Fonction pour rechercher un code ASCII dans un Trie et renvoyer la chaîne correspondante
int Recherche_un_noeud(struct Trie* head, int code, uint32_t* str)
{
    struct Trie* node;
    
    if (head == NULL ) {
        return 0;
    }    
    //printf("code = %d\n",code);
    node = Recherche_dans_l_arbre(head, code);
    if (node != NULL) {
        //printf("node = %p car %u %u %u\n",node,node->caractere[0],node->caractere[1],node->caractere[2]);
        memcpy(str,node->caractere,quelle_taille(node->caractere)*sizeof(uint32_t));
        return 1;
    }
    
    if (node == NULL) {
        return 0;
    }
    return 1;
}

int CompteurNoeuds(struct Trie* head)
{
    if (head == NULL) {
        return 0;
    }

    int count = 1; // Compte le nœud courant

    for (int i = 0; i < CHAR_SIZE; i++) {
        if (head->character[i] != NULL)
            count += CompteurNoeuds(head->character[i]); // Compte les nœuds des enfants récursivement
    }

    return count;
}

void Liberation(struct Trie* head)
{
    for (int i = 0; i < CHAR_SIZE; i++) {
        if (head->character[i] != NULL)
            Liberation(head->character[i]);
    }

    if (head != NULL)
    {
        //printf("Liberation OK %d\n",head->code_ascii);
        free(head);
    }
            
 
  
}