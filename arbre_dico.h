#include <stdio.h>
#include <stdint.h>

#define CHAR_SIZE 512

struct Trie
{
    int isLeaf;             // 1 lorsque le nœud est un nœud feuille
    struct Trie* character[CHAR_SIZE];
    int code_ascii;
    uint16_t caractere[24];
};
 

int quelle_taille(uint16_t * chaine) ;

int compare(uint16_t * x,uint16_t * y);

struct Trie* getNewTrieNode(int code);
 
void insert(struct Trie *head, uint16_t * str, int code);
 
int search(struct Trie* head, uint16_t* str);

int hasChildren(struct Trie* curr);

int deletion(struct Trie **curr, uint16_t* str);

int search_code(struct Trie* head, uint16_t* str);

int search_node(struct Trie* head, int code, uint16_t* str);
