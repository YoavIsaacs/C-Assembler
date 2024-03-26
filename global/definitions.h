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
#define MAX_DATA_SIZE 80
#define MAX_NUM_OF_OPERANDS 2
#define TEMP_INPUT_LENGTH 2048
#define MAX_INT_SIZE 10
#define BASE_TEN 10
#define NUMBER_OF_OPERANDS 2
#define NUMBER_OF_INSTRUCTIONS 16
#define BOTTOM_REGISTER 0
#define TOP_REGISTER 7
#define BREAKERS " \t\v,"
#define FOREVER 1
#define NUMBER_OF_VALID_OPERANDS_FOR_DEFINE 4
#define NUMBER_OF_VALID_OPERANDS_FOR_TWO_OPERAND_INSTRUCTIONS 4
#define NUMBER_OF_VALID_OPERANDS_FOR_ONE_OPERAND_INSTRUCTIONS 2

/* SYNTAX ERRORS */

#define LINE_TOO_LONG_ERROR "Error, input line is too long.\n"
#define MEMORY_ALLOCATION_ERROR "Error, memory allocation failed.\n"
#define LABEL_TOO_LONG_ERROR "Error, label is too long.\n"
#define UNKNOWN_COMMAND_ERROR "Error, unknown command.\n"
#define ILLEGAL_LABEL_NAME_ERROR "Error, illegal lable name.\n"
#define ILLEGAL_STRING_ERROR "Error, illegal string.\n"
#define INVALID_NUMBER_ERROR "Error, illegal number detected.\n"
#define INVALID_END_OF_LINE_ERROR "Error, line cannot end with a comma.\n"
#define EXCESS_COMMA_ERROR "Error, excess commas in command.\n"
#define MISSING_COMMA_ERROR "Error, missing comma between numbers.\n"
#define NUMBER_TOO_LONG_ERROR "Error, number is too large.\n"
#define INVALID_LABEL_ERROR "Error, invalid label.\n"
#define NUMBER_OUT_OF_BOUNDS_ERROR "Error, number out of bounds.\n"
#define INVALID_REGISTER_ERROR "Error, invalid register.\n"
#define INVALID_INDEX_ERROR "Error, invalid index.\n"
#define INVALID_STRING_ERROR "Error, invalid string.\n"
#define EXCESS_STRINGS_ERROR "Error, extra text after the end of the line.\n"
#define INVALID_DEFINE_ERROR "Error, invalid number of operands for .define.\n"
#define INVALID_INSTRUCTION_ERROR "Error, invalid number of operands for instruction.\n"
#define INVALID_OPERAND_ERROR "Error, invalid operand.\n"

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

enum {
  VALID_NUMBER,
  INVALID_NUMBER,
  VALID_NUMBER_TOO_BIG_OR_TOO_SMALL
};

enum {
  VALID_LABEL,
  INVALID_LABEL
};

enum {
  AST_opcode_mov,
  AST_opcode_cmp,
  AST_opcode_add,
  AST_opcode_sub,
  AST_opcode_not,
  AST_opcode_clr,
  AST_opcode_lea,
  AST_opcode_inc,
  AST_opcode_dec,
  AST_opcode_jmp,
  AST_opcode_bne,
  AST_opcode_red,
  AST_opcode_prn,
  AST_opcode_jsr,
  AST_opcode_rts,
  AST_opcode_hlt
};

enum {
  operand_source,
  operand_destination
};

enum {
  INVALID_OPERAND,
  CONSTANT,
  REGISTER,
  LABEL,
  INDEX
};

enum { 
  DATA,
  STRING,
  ENTRY,
  EXTERN,
  DEFINE,
  INVALID_DIRECTIVE
};


typedef struct separated_strings_from_input_line {
  char *separated_strings[MAX_LINE_LENGTH];
  int number_of_strings;
  int error;
}separated_strings_from_input_line;


typedef struct assembler_AST {
  char error[MAX_ERROR_LENGTH];
  enum { AST_instruction, AST_directive, AST_define, AST_error } AST_type;
  char label[MAX_LABEL_LENGTH];

  union {

    struct {
      enum {
        AST_instruction_mov,
        AST_instruction_cmp,
        AST_instruction_add,
        AST_instruction_sub,
        AST_instruction_not,
        AST_instruction_clr,
        AST_instruction_lea,
        AST_instruction_ind,
        AST_instruction_dec,
        AST_instruction_jmp,
        AST_instruction_bne,
        AST_instruction_red,
        AST_instruction_prn,
        AST_instruction_jsr,
        AST_instruction_rts,
        AST_instruction_hlt
      }AST_instruction_option;

      struct {
        enum {
          AST_instruction_operand_option_constant,
          AST_instruction_operand_option_label,
          AST_instruction_operand_option_index_constant,
          AST_instruction_operand_option_index_label,
          AST_instruction_operand_option_index_register
        }AST_instruction_operand_option;
        union {
          int constant;
          char label[MAX_LABEL_LENGTH];
          int register_number;
          struct {
            char label[MAX_LABEL_LENGTH];
            union {
              int constant;
              char label[MAX_LABEL_LENGTH];
            }index_type;
          }index;
        }AST_instruction_operand_type;
      }AST_instruction_operand_type[NUMBER_OF_OPERANDS];

    }instruction;


    struct {
      enum {
        AST_directive_data,
        AST_directive_string,
        AST_directive_entry,
        AST_directive_extern
      } AST_directive_type;

      union {
        struct {
          enum {
            AST_directive_data_constant,
            AST_directive_data_label
          } AST_directive_data_options;
          union {
            char label[MAX_LABEL_LENGTH];
            int constant;
          } AST_directive_data_type[MAX_DATA_SIZE];
        } AST_directive_data;
        char string[MAX_LABEL_LENGTH];
        char label[MAX_LABEL_LENGTH];
      } AST_directive_options;
    } directive;

    struct {
      char label[MAX_LABEL_LENGTH];
      int number;
    }define;
  } AST_options;
} assembler_AST;







#endif
