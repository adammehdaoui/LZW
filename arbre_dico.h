#include <stdio.h>
#include <stdint.h>

#define CHAR_SIZE 512

struct Trie
{
    struct Trie* character[CHAR_SIZE];
    int code_ascii;
};

/**
 * @brief Crée une feuille du Trie.
 *
 * Cette fonction crée une nouvelle feuille de l'arbre Trie avec le code ASCII et + spécifié.
 *
 * @param code Le code ASCII et + à assigner à la feuille de l'arbre Trie.
 * @return Un pointeur vers la nouvelle feuille du Trie créée.
 */ 
struct Trie* CreationFeuille(int code);

/**
 * @brief Insère une chaîne de caractères dans l'arbre Trie de manière itérative.
 *
 * Cette fonction insère une chaîne de caractères dans le Trie à partir de la racine spécifiée.
 *
 * @param racine La racine du Trie dans lequel insérer la chaîne de caractères.
 * @param str La chaîne de caractères à insérer dans le Trie.
 * @param code Le code à associer à la chaîne de caractères dans le Trie.
 * @param taille La taille de la chaîne de caractères à insérer (permet la gestion de NULL)
 */ 
void insert(struct Trie *racine, uint32_t * str, int code, int taille);

/**
 * @brief Recherche un code dans l'arbre Trie de manière itérative.
 *
 * Cette fonction recherche une chaîne de caractères dans le Trie à partir de la racine spécifiée et renvoie le code associé à la chaîne de caractères si elle est trouvée dans le Trie. Sinon, elle renvoie -1.
 *
 * @param racine La racine du Trie dans lequel effectuer la recherche.
 * @param str Le tableau de caractères à rechercher dans le Trie.
 * @param taille La taille de la chaîne de caractères à rechercher.
 * @return Le code associé à la chaîne de caractères si elle est trouvée dans le Trie ; -1 sinon.
 */  
int Recherche_code_dans_l_arbre(struct Trie* racine, uint32_t* str, int taille);

/**
 * @brief Libère la mémoire allouée pour un arbre Trie de manière récursive.
 *
 * Cette fonction libère la mémoire allouée pour un Trie en parcourant le Trie de manière récursive, en commençant par la racine spécifiée.
 *
 * @param racine La racine du Trie à libérer.
 */
void Liberation(struct Trie* racine);

/**
 * @brief Initialise le dictionnaire.
 *
 * Cette fonction crée et initialise un dictionnaire Trie avec les codes ASCII 0 à 256.
 *
 * @return Un pointeur vers la racine du dictionnaire Trie initialisé.
 */
struct Trie * Initialisation_dico();


