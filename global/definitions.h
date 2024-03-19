#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* DEFINITIONS */
#define MAX_LINE_LENGTH 81
#define MAX_BUFFER_LENGTH 1024
#define ASSEMBLY_FILE_EXTENSION ".as"
#define POST_PREPROCESSOR_FILE_EXTENSION ".am"
#define MACRO_START_LENGTH 4
#define LINE_INCEREMENTS 10
#define INITIAL_TABLE_SIZE 100


/* ENUMS */
enum BOOL {
    NO,
    YES
};
/**
 * @brief The possible states of a line in the preprocessor
 * 
 */
enum possibleLineStates {start, stop, call, comment, other, empty, error};



/* TRIE */
/**
 * @brief The number of possible characters in the trie
 * 
 */
#define POSSIBLE_CHARACTERS 95

/**
 * @brief The base character for the trie
 * 
 */
#define TRIE_BASE_CHAR ' '

/**
 * @brief The trie node structure
 * 
 */
typedef struct TrieNode {
  struct TrieNode *next[POSSIBLE_CHARACTERS];
  int isEndOfWord;
  char *name;
} TrieNode;




/* MACRO */
/**
 * @brief The macro structure
 * 
 */
typedef struct Macro {
  int error;
  char *name;
  char **lines;
  int numOfLines;
} Macro;

/* LINKED_LIST */
/**
 * @brief The node structure
 * 
 */
typedef struct node {
  Macro *data;
  struct node *next;
} Node;

/**
 * @brief The list structure
 * 
 */
typedef struct list {
  int size;
  Node *head;
} List;




/* HASH TABLE */

typedef struct Entry {
    char* key;
    void* value;
    struct Entry* next;
} Entry;

typedef struct HashTable {
    Entry** entries;
} HashTable;

HashTable* create_table();














/* PREPROCESSOR */



#endif