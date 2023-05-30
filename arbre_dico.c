#include "arbre_dico.h"
#include <stdlib.h>
#include <string.h>

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
void insert(struct Trie *head, uint32_t * str, int code, int taille)
{
    // partir du nœud racine
    struct Trie* curr = head;
    int i=0;
    int compteur = taille;

    printf("creation noeud %u %u %u %u %u %u %u %u %u %u\n",str[0],str[1],str[2],str[3],str[4],str[5],str[6],str[7],str[8],str[9]);

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
        curr = curr->character[str[i]];
 
        if (curr == NULL) 
            return 0;
        i++;
    }
 
    return 1;
}


 int Recherche_code_dans_l_arbre(struct Trie* head, uint32_t* str, int taille)
{
    int i=0;
    int compteur = taille;
    
    // retourne 0 si Trie est vide
    if (head == NULL) {
        printf("head null\n");
        return -1;
    }
 
    struct Trie* curr = head;
    while (compteur>0)
    {
        printf("while %d code %d compt %d\n",i, curr->code_ascii,compteur);
        // passe au nœud suivant
        curr = curr->character[str[i]];
    
        // si la string est invalide (a atteint la fin d'un chemin dans le Trie)
        if (curr == NULL) {
            printf("on sort ici\n");
            return -1;
        }
 
        // passe au caractère suivant
        i++;
        compteur--;
    }
 
    // renvoie 1 si le noeud courant est une feuille et que le
    // la fin de la string est atteinte
    printf("on arrive ici\n");
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
        printf("node null\n");
        return NULL;
    }

    //printf("code_ascii %d et code %d\n",node->code_ascii,code);

    // Si le nœud courant est une feuille et son code correspond au code recherché
    if (node->code_ascii == code)
    {
        printf("sortie trouvé\n");
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
int Recherche_un_noeud(struct Trie* head, int code, uint32_t* str)
{
    struct Trie* node;
    struct Trie* node_enfant;
    struct Trie* node_parent;
    
    int enieme = 0;
    int i = 0;
    uint32_t chn[24];
    
    if (head == NULL ) {
        printf("head null\n");
        return 0;
    }    
    
    memset(chn,0,24*sizeof(uint32_t));
    node = Recherche_dans_l_arbre(head, code);
    printf("node %p \n",node);
    
    i = CompteurNiveaux(head,node)-2;
    node_enfant = node;
    while(node_enfant != NULL){
        node_parent = Trouver_Noeud_Pere(head, node_enfant);
        enieme = Lien_character_pere_fils(node_parent, node_enfant);
        chn[i] = enieme;
        node_enfant = node_parent;
        i--;
    }
    printf("chn %u %u\n",chn[0],chn[1]);
    if (node != NULL) {
        printf("on passe dans le if\n");
        memcpy(str,chn, (CompteurNiveaux(head,node)-1) * sizeof(uint32_t));
        return 1;
    }
    else return 0;

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
        free(head);
    }
  
}

struct Trie* Trouver_Noeud_Pere(struct Trie* head, struct Trie* child)
{
    if (head == NULL || child == NULL)
        return NULL;
    
    for (int i = 0; i < CHAR_SIZE; i++) {
        if (head->character[i] == child)
            return head;
        
        if (head->character[i] != NULL) {
            struct Trie* node = Trouver_Noeud_Pere(head->character[i], child);
            if (node != NULL)
                return node;
        }
    }
    
    return NULL;
}

int Lien_character_pere_fils(struct Trie* pere, struct Trie* fils)
{
    if (fils == NULL || pere == NULL) {
        //printf("on passe dans le if\n");
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

int CompteurNiveaux(struct Trie * head,struct Trie * node){
    int i=0;
    struct Trie * node_enfant;
    struct Trie * node_parent;
    
    if (head == NULL || node == NULL) {
        return 0;
    }    
    
    node_enfant = node;
    while(node_enfant != NULL){
        node_parent = Trouver_Noeud_Pere(head, node_enfant);
        node_enfant = node_parent;
        i++;
    }
    
    return i;
}