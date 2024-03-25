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

int parse_operand(char *input, int instruction, int source_destination,
                  assembler_AST *AST, TrieNode *keywords) {
  int *operand;
  int check;
  int label_name_length;
  char *temp;
  char *first_letter_of_index;
  char label[MAX_LABEL_LENGTH];
  char temp_label[MAX_LABEL_LENGTH];
  union possible_operand {
    int constant;
    char label[MAX_LABEL_LENGTH];
  };

  operand = NULL;

  /* CHECK IF CONSTANT */
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
      return CONSTANT;
    case INVALID_NUMBER:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_NUMBER_ERROR);
      return INVALID_OPERAND;
    case VALID_NUMBER_TOO_BIG_OR_TOO_SMALL:
      AST->AST_type = AST_error;
      strcpy(AST->error, NUMBER_OUT_OF_BOUNDS_ERROR);
      return INVALID_OPERAND;
    default:
      break;
    }
  }

  /* CHECK IF REGISTER */
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
      return REGISTER;
    case INVALID_NUMBER:
    case VALID_NUMBER_TOO_BIG_OR_TOO_SMALL:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_REGISTER_ERROR);
      return INVALID_OPERAND;
    default:
      break;
    }
  }

  temp = strchr(input, '[');

  /* CHECK IF ARRAY */

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
      return LABEL;
    case INVALID_LABEL:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      return INVALID_OPERAND;
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
      first_letter_of_index = temp + 2;
      temp = strchr(temp, ']');
      if (temp == NULL) {
        AST->AST_type = AST_error;
        strcpy(AST->error, INVALID_INDEX_ERROR);
        return INVALID_OPERAND;
      }
      temp--;
      label_name_length = temp - first_letter_of_index;
      strncpy(temp_label, first_letter_of_index, label_name_length);
      check = does_label_jave_only_digits(temp_label);
      switch (check) {
      case YES:
        AST->AST_options.instruction
            .AST_instruction_operand_type[source_destination]
            .AST_instruction_operand_type.index.index_type.constant =
            (int)strtol(temp_label, NULL, BASE_TEN);
        AST->AST_options.instruction
            .AST_instruction_operand_type[source_destination]
            .AST_instruction_operand_option =
            AST_instruction_operand_option_index_constant;
        break;
      case NO:
        check = is_a_valid_label(temp_label, keywords);
        switch (check) {
        case VALID_LABEL:
          strcpy(AST->AST_options.instruction
                     .AST_instruction_operand_type[source_destination]
                     .AST_instruction_operand_type.index.index_type.label,
                 temp_label);
          AST->AST_options.instruction
              .AST_instruction_operand_type[source_destination]
              .AST_instruction_operand_option =
              AST_instruction_operand_option_index_label;
          return INDEX;
        case INVALID_LABEL:
          AST->AST_type = AST_error;
          strcpy(AST->error, INVALID_INDEX_ERROR);
          return INVALID_OPERAND;
        }
        break;
      }
      break;
    case INVALID_LABEL:
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
    }
  }
  return INVALID_OPERAND;
}

int does_label_jave_only_digits(char *input) {
  while (*input != '\0') {
    if (!isdigit(*input)) {
      return NO;
    }
    input++;
  }
  return YES;
}

/*
separated_strings_from_input_line separate_input_line(char *input) {
  int number_of_strings;
  int was_separated_successfully;
  separated_strings_from_input_line separated_strings = {0};
  char *temp;

  was_separated_successfully = remove_spaces_within_brackets(input, input);
  if (was_separated_successfully == NO) {
    separated_strings.error = YES;
    return separated_strings;
  }

  separated_strings.error = NO;

  while (isspace(*input)) {
    input++;
  }

  if (*input == '\0') {
    return separated_strings;
  }

  number_of_strings = 0;
  
  do {
    separated_strings.separated_strings[number_of_strings++] = input;
    temp = strpbrk(input, BREAKERS);
    
    if (temp) {
      *temp = '\0';
      temp++;
      while (isspace(*temp))
        temp++;
      if (*temp == '\0') {
        break;
      }
      input = temp;
    } else {
      break;
    }
  } while (FOREVER);
      
    separated_strings.number_of_strings = number_of_strings;
    return separated_strings;
  }
*/

separated_strings_from_input_line separate_input_line(char *input) {
    int number_of_strings;
    int was_separated_successfully;
    separated_strings_from_input_line separated_strings = {0};
    char *temp;

    was_separated_successfully = remove_spaces_within_brackets(input, input);
    if (was_separated_successfully == NO) {
        separated_strings.error = YES;
        return separated_strings;
    }

    separated_strings.error = NO;

    while (isspace(*input)) {
        input++;
    }

    if (*input == '\0') {
        return separated_strings;
    }

    number_of_strings = 0;

    do {
            separated_strings.separated_strings[number_of_strings++] = input;
        temp = strpbrk(input, BREAKERS);

        if (temp && *temp != ',') {
            *temp = '\0';
            temp++;
            while (isspace(*temp))
                temp++;
            if (*temp == '\0') {
                break;
            }
            input = temp;
        } else if (temp && *temp == ',') {
            *temp = '\0';
            separated_strings.separated_strings[number_of_strings++] = ",";
            temp++;
            while (isspace(*temp))
                temp++;
            if (*temp == '\0') {
                break;
            }
            input = temp;
        } else {
            break;
        }
    } while (FOREVER);

    separated_strings.number_of_strings = number_of_strings;
    return separated_strings;
}


int remove_spaces_within_brackets(char *input, char *output) {
  char *temp;
  char output_temp[MAX_LINE_LENGTH] = {0};
  int i;
  int within_brackets = NO;
  int first_word = NO;
  int first_string_ended = NO;

  i = 0;
  temp = input;
  while (*temp != '\0') {

    if (*temp == '[') {
      within_brackets = YES;
      first_word = YES;
      first_string_ended = NO;
      output_temp[i] = *temp;
      i++;
      temp++;
      continue;
    }
    if (*temp == ']') {
      within_brackets = NO;
      first_string_ended = NO;
      output_temp[i] = *temp;
      i++;
      temp++;
      continue;
    }
    if (within_brackets == YES) {
      if (isspace(*temp) && first_string_ended == NO) {
        temp++;
        continue;
      }
      if (!isspace(*temp) && first_string_ended == NO) {
        output_temp[i] = *temp;
        i++;
        temp++;
        continue;
      }
      if (isspace(*temp) && first_string_ended == YES) {
        temp++;
        continue;
      }
      if (isspace(*input) && first_word == YES) {
        first_string_ended = YES;
        temp++;
        continue;
      }
      if (!isspace(*temp) && first_string_ended == YES) {
        return NO;
      }
    }

    output_temp[i] = *temp;
    i++;
    temp++;
  }
  strcpy(output, output_temp);
  return YES;
}