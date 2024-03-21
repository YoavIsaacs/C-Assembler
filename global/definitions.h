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
#define MAX_ERROR_LENGTH 300
#define MAX_LABEL_LENGTH 32
#define MAX_DATA_SIZE 1000
#define MAX_NUM_OF_OPERANDS 2
#define TEMP_INPUT_LENGTH 2048
#define MAX_INT_SIZE 10
#define BASE_TEN 10


/* SYNTAX ERRORS */

#define LINE_TOO_LONG_ERROR "Error, input line is too long.\n"
#define MEMORY_ALLOCATION_ERROR "Error, memory allocation failed.\n"
#define LABEL_TOO_LONG_ERROR "Error, label is too long.\n"
#define UNKNOWN_COMMAND_ERROR "Error, unknown command.\n"
#define ILLEGAL_LABEL_NAME_ERROR "Error, illegal lable name.\n"
#define ILLEGAL_STRING_ERROR "Error, illegal string.\n"
#define INVALID_NUMBER_ERROR "Error, illegal number detected.\n"
#define INVALID_END_OF_LINE_ERROR "Error, line cannot end with a comma.\n"
#define DOUBLE_COMMA_ERROR "Error, consecutive commas.\n"
#define MISSING_COMMA_ERROR "Error, missing comma between numbers.\n"
#define NUMBER_TOO_LONG_ERROR "Error, number is too large.\n"

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













/* FRONDEND */

typedef struct assembler_AST {
  int has_error;
  char error[MAX_ERROR_LENGTH];
  char label_name[MAX_LABEL_LENGTH];
  enum { instruction, directive } AST_opt;

  union {
    struct {
      enum { dir_data, dir_string, dir_entry, dir_extern } dir_Opt;

      union {
        char *entry_extern_label;
        char *string;
        struct {
          int data[MAX_DATA_SIZE];
          int data_count;
        } data;
      } directive_operand;
    } directive;

    struct {
      enum {
        mov,
        cmp,
        add,
        sub,
        Not,
        clr,
        lea,
        inc,
        dec,
        jmp,
        bne,
        red,
        prn,
        jsr,
        rts,
        hlt
      } possible_command;
      enum {
        no_operands,
        const_operand,    /* CONSTANT NUMBER */
        label_operand,    /* VARIABLE */
        register_operand, /* REGISTER */
        indexed_operand   /* ARRAY */
      } operand_options[MAX_NUM_OF_OPERANDS];
      union {
        int const_number;
        int register_number;
        char *label;
        struct {
          union {
            struct {
              char *label;
              int index;
            }constant;
            struct {
              char *array;
              char *label;
              int index;
            }labeled_index;
          }possible_index;
        }index;
      } operands[MAX_NUM_OF_OPERANDS];
    } instruction;
  } AST_type;

} assembler_AST;
#endif
