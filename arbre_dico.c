#include "arbre_dico.h"
#include <stdlib.h>
#include <string.h>

#define TAILLE_CHN_TEMP 30

// Fonction qui renvoie un nouveau noeud Trie
struct Trie* CreationFeuille(int code)
{
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    //printf("creation noeud %p  %d\n",node,code);
    node->code_ascii = code;
    
    for (int i = 0; i < CHAR_SIZE; i++) {
        node->character[i] = NULL;
    }

    return node;
}
 
// Fonction itérative pour insérer une string dans un Trie
void insert(struct Trie *racine, uint32_t * str, int code, int taille)
{
    // partir du nœud racine
    struct Trie* curr = racine;
    int i=0;
    int compteur = taille;

    while (compteur  > 0)
    {
        //printf("dans le while %u %d\n",str[i],i);
        // crée un nouveau nœud si le chemin n'existe pas
        if (curr->character[str[i]] == NULL) 
            curr->character[str[i]] = CreationFeuille(code);
 
        // passe au nœud suivant
        curr = curr->character[str[i]];
 
        // passe au caractère suivant
        i++;
        compteur--;
    }
 
}
 
// Fonction itérative pour rechercher un code dans un Trie. Il renvoie le code
// si la string est trouvée dans le Trie ; sinon, il renvoie -1.
 int Recherche_code_dans_l_arbre(struct Trie* racine, uint32_t* str, int taille)
{
    int i=0;
    int compteur = taille;
    
    // retourne 0 si Trie est vide
    if (racine == NULL) {
        return -1;
    }
 
    struct Trie* curr = racine;
    while (compteur>0)
    {
        //printf("while %d code %d compt %d\n",i, curr->code_ascii,compteur);
        // passe au nœud suivant
        curr = curr->character[str[i]];
    
        // si la string est invalide (a atteint la fin d'un chemin dans le Trie)
        if (curr == NULL) {
            return -1;
        }
 
        // passe au caractère suivant
        i++;
        compteur--;
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

// Fonction récursive pour rechercher un code dans un Trie et renvoyer la chaîne correspondante
struct Trie* Recherche_dans_l_arbre(struct Trie* node, int code)
{
    if (node == NULL) {
        return NULL;
    }

    //printf("code_ascii %d et code %d\n",node->code_ascii,code);

    // Si le nœud courant est une feuille et son code correspond au code recherché
    if (node->code_ascii == code)
    {
        return node;
    }

    // Parcourt les caractères suivants dans le Trie
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (node->character[i] != NULL)
        {
            //printf("ici valeur suivante %d\n",(node->character[i])->code_ascii);
            // Appelle récursivement la recherche pour les nœuds enfants
            struct Trie* result = Recherche_dans_l_arbre(node->character[i], code);
            if (result != NULL) {
                return result;
            }
        }
    }

    return NULL;
}

// Fonction pour rechercher un code ASCII dans un Trie et renvoyer la chaîne correspondante
int Recherche_un_noeud(struct Trie* racine, int code, uint32_t* str)
{
    struct Trie* node;
    struct Trie* node_enfant;
    struct Trie* node_parent;
    
    int enieme = 0;
    int i = 0;
    uint32_t chn[TAILLE_CHN_TEMP];
    
    if (racine == NULL ) {
        return 0;
    }    
    
    memset(chn,0,TAILLE_CHN_TEMP*sizeof(uint32_t));
    node = Recherche_dans_l_arbre(racine, code);
    
    i = CompteurNiveaux(racine,node)-2;
    node_enfant = node;
    while(node_enfant != NULL){
        node_parent = Trouver_Noeud_Pere(racine, node_enfant);
        enieme = Lien_character_pere_fils(node_parent, node_enfant);
        chn[i] = enieme;
        node_enfant = node_parent;
        i--;
    }
    
    if (node != NULL) {
        memcpy(str,chn, (CompteurNiveaux(racine,node)-1) * sizeof(uint32_t));
        return 1;
    }
    else return 0;

}

// Fonction récursive pour compter le nombre de nœuds dans un Trie
int CompteurNoeuds(struct Trie* racine)
{
    if (racine == NULL) {
        return 0;
    }

    int count = 1; // Compte le nœud courant

    for (int i = 0; i < CHAR_SIZE; i++) {
        if (racine->character[i] != NULL)
            count += CompteurNoeuds(racine->character[i]); // Compte les nœuds des enfants récursivement
    }

    return count;
}

// Fonction récursive pour libérer la mémoire allouée à un Trie
void Liberation(struct Trie* racine)
{
    for (int i = 0; i < CHAR_SIZE; i++) {
        if (racine->character[i] != NULL)
            Liberation(racine->character[i]);
    }

    if (racine != NULL)
    {
        free(racine);
    }
  
}

// Fonction récursive pour trouver le nœud père d'un nœud donné
struct Trie* Trouver_Noeud_Pere(struct Trie* racine, struct Trie* fils)
{
    if (racine == NULL || fils == NULL)
        return NULL;
    
    for (int i = 0; i < CHAR_SIZE; i++) {
        if (racine->character[i] == fils)
            return racine;
        
        if (racine->character[i] != NULL) {
            struct Trie* node = Trouver_Noeud_Pere(racine->character[i], fils);
            if (node != NULL)
                return node;
        }
    }
    
    return NULL;
}

// Fonction récursive pour trouver le lien Character entre un nœud père et un nœud fils
int Lien_character_pere_fils(struct Trie* pere, struct Trie* fils)
{
    if (fils == NULL || pere == NULL) {
        return 0;
    }   

    for (int i = 0; i < CHAR_SIZE; i++) {
        if (pere->character[i] != NULL) {
            if (pere->character[i] == fils)
                return i;
        }
    }

    return 0;
}

// Fonction récursive pour compter le nombre de niveaux entre un nœud et la racine
int CompteurNiveaux(struct Trie * racine,struct Trie * node){
    int i=0;
    struct Trie * node_enfant;
    struct Trie * node_parent;
    
    if (racine == NULL || node == NULL) {
        return 0;
    }    
    
    node_enfant = node;
    while(node_enfant != NULL){
        node_parent = Trouver_Noeud_Pere(racine, node_enfant);
        node_enfant = node_parent;
        i++;
    }
    
    return i;
}
