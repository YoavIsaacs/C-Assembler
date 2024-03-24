#ifndef _ASSEMBLER_AST
#define _ASSEMBLER_AST

#include "../global/definitions.h"
#include "../Preprocessor/preprocessor.h"
#include <stdio.h>

assembler_AST *create_assembler_AST(char *);


/**
 * @brief This function checks if a number is a valid number. The validity depends on the context of the call - for registers it'll be 0-7 etc.
 *        The parsed number is stored in the pointer passed as the last argument. 
 *
 * @return VALID_NUMBER if it's a valid number, INVALID_NUMBER if it's invalid, VALID_NUMBER_TOO_BIG_OR_TOO_SMALL if it's a valid number but it's too big or too small
  
 */
int is_a_valid_number(char *, int, int, int *);

/**
 * @brief This finction counts and returns the number of digits in a number.
 * 
 * @return the number of digits in the number
 */
int check_number_of_digits(int);

/**
 * @brief This function checks if a label is a valid label.
 * 
 * @return VALID_LABEL if it's a valid label, INVALID_LABEL if it's invalid.
 */
int is_a_valid_label(char *, TrieNode *);


/**
 * @brief Fill a trie with the keywords of the assembly language.
 * 
 */
void create_keywords(TrieNode *);


/**
 * @brief This function parses an operand and stores it in the AST.
 * 
 */
void parse_operand(char *, int, int, assembler_AST *, TrieNode *);

#endif
