#include "assemblerAST.h"
#include "../global/definitions.h"
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct allowed_operands_for_instructions {
  char *instruction_name;
  int instruction_opcode;
  char *allowed_source;
  char *allowed_destination;
};

struct allowed_operands_for_instructions
    instuction_operand_table[NUMBER_OF_INSTRUCTIONS] = {
        {"mov", AST_opcode_mov, "0123", "123"},
        {"cmp", AST_opcode_cmp, "0123", "0123"},
        {"add", AST_opcode_add, "0123", "123"},
        {"sub", AST_opcode_sub, "0123", "123"},
        {"not", AST_opcode_not, "", "123"},
        {"clr", AST_opcode_clr, "", "123"},
        {"lea", AST_opcode_lea, "12", "123"},
        {"inc", AST_opcode_inc, "", "123"},
        {"dec", AST_opcode_dec, "", "123"},
        {"jmp", AST_opcode_jmp, "", "13"},
        {"bne", AST_opcode_bne, "", "13"},
        {"red", AST_opcode_red, "", "123"},
        {"prn", AST_opcode_prn, "", "0123"},
        {"jsr", AST_opcode_jsr, "", "13"},
        {"rts", AST_opcode_rts, "", ""},
        {"hlt", AST_opcode_hlt, "", ""},
};

assembler_AST *create_assembler_AST(char *input) {}

int is_a_valid_number(char *input, int lower_bound, int upper_bound,
                      int *result) {
  int input_length;
  int parsed_number;

  if (*input == 'r')
    input++;

  input_length = strlen(input);
  parsed_number = strtol(input, &input, BASE_TEN);
  if (input_length != check_number_of_digits(parsed_number)) {
    return INVALID_NUMBER;
  }

  if (parsed_number < lower_bound || parsed_number > upper_bound) {
    return INVALID_NUMBER;
  }

  *result = (int)parsed_number;
  return VALID_NUMBER;
}

int check_number_of_digits(int number) {
  int i;
  for (i = 0; number != 0; i++)
    number /= 10;
  return i;
}

int is_a_valid_label(char *input, TrieNode *keywords) {

  int label_length;

  if (!isalpha(*input)) {
    return INVALID_LABEL;
  }

  label_length = strlen(input);
  if (label_length > MAX_LABEL_LENGTH) {
    return INVALID_LABEL;
  }

  if (search_trie(keywords, input) == YES) {
    return INVALID_LABEL;
  }

  return VALID_LABEL;
}

void create_keywords(TrieNode *keywords) {
  insert_trie(keywords, "mov", "mov");
  insert_trie(keywords, "cmp", "cmp");
  insert_trie(keywords, "add", "add");
  insert_trie(keywords, "sub", "sub");
  insert_trie(keywords, "not", "not");
  insert_trie(keywords, "clr", "cle");
  insert_trie(keywords, "lea", "lea");
  insert_trie(keywords, "inc", "inc");
  insert_trie(keywords, "dec", "dec");
  insert_trie(keywords, "jmp", "jmo");
  insert_trie(keywords, "bne", "bne");
  insert_trie(keywords, "red", "red");
  insert_trie(keywords, "prn", "prn");
  insert_trie(keywords, "jsr", "jsr");
  insert_trie(keywords, "rts", "rts");
  insert_trie(keywords, "hlt", "hlt");
  insert_trie(keywords, "mcr", "mcr");
  insert_trie(keywords, "end_mcr", "end_mcr");
}

void parse_operand(char *input, int instruction, int source_destination,
                   assembler_AST *AST, TrieNode *keywords) {
  int *operand;
  int check;
  int label_name_length;
  char *temp;
  char label[MAX_LABEL_LENGTH];
  char temp_label[MAX_LABEL_LENGTH];
  union possible_operand {
    int constant;
    char label[MAX_LABEL_LENGTH];
  };

  if (*input == '#') {
    input++;
    check = is_a_valid_number(input, INT_MIN, INT_MAX, operand);
    switch (check) {
    case VALID_NUMBER:
      AST->AST_options.instruction
          .AST_instruction_operand_type[source_destination]
          .AST_instruction_operand_type.index.index_type.constant = *operand;
      AST->AST_options.instruction
          .AST_instruction_operand_type[source_destination]
          .AST_instruction_operand_option =
          AST_instruction_operand_option_constant;
      break;
    case INVALID_NUMBER:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_NUMBER_ERROR);
      break;
    case VALID_NUMBER_TOO_BIG_OR_TOO_SMALL:
      AST->AST_type = AST_error;
      strcpy(AST->error, NUMBER_OUT_OF_BOUNDS_ERROR);
      break;
    default:
      break;
    }
  }

  if (*input == 'r' && isdigit(*(input + 1))) {
    input++;
    check = is_a_valid_number(input, BOTTOM_REGISTER, TOP_REGISTER, operand);
    switch (check) {
    case VALID_NUMBER:
      AST->AST_options.instruction
          .AST_instruction_operand_type[source_destination]
          .AST_instruction_operand_type.register_number = *operand;
      AST->AST_options.instruction
          .AST_instruction_operand_type[source_destination]
          .AST_instruction_operand_option =
          AST_instruction_operand_option_index_register;
      break;
    case INVALID_NUMBER:
    case VALID_NUMBER_TOO_BIG_OR_TOO_SMALL:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_REGISTER_ERROR);
      break;
    default:
      break;
    }
  }

  temp = strchr(input, '[');
  if (temp == NULL) {
    check = is_a_valid_label(input, keywords);
    switch (check) {
    case VALID_LABEL:
      AST->AST_options.instruction
          .AST_instruction_operand_type[source_destination]
          .AST_instruction_operand_option =
          AST_instruction_operand_option_label;
      strcpy(AST->AST_options.instruction
                 .AST_instruction_operand_type[source_destination]
                 .AST_instruction_operand_type.label,
             input);
      break;
    case INVALID_LABEL:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      break;
    default:
      break;
    }
  } else {
    temp--;
    label_name_length = temp - input;
    strncpy(label, input, label_name_length);
    check = is_a_valid_label(label, keywords);
    switch (check) {
    case VALID_LABEL:
      strcpy(AST->AST_options.instruction
                 .AST_instruction_operand_type[source_destination]
                 .AST_instruction_operand_type.index.label,
             label);

        temp = strchr(temp, ']');
        if (temp == NULL) {
          AST->AST_type = AST_error;
          strcpy(AST->error, INVALID_INDEX_ERROR);
          return;
        }
        temp--;


      break;
    case INVALID_LABEL:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      break;
    }

    /*

      CONTINUE FROM HERE WITH AN ARRAY WITH A POSSIBLE INDEX OR LABEL!

    */
  }
