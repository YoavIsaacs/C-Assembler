#include "assemblerAST.h"
#include "../global/definitions.h"
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>



assembler_AST *create_assembler_AST(char *input) {

  int substring_number = 0;
  int substring_length;
  int checker;
  int temp_number;
  int i;
  int data_count;
  int num_of_commas;
  int opcode;
  int has_label_to_check_for_allowed_num_of_strings;
  int *operand;
  char *temp_string;
  char define_name[MAX_LABEL_LENGTH] = {0};
  
  TrieNode *keywords;
  
  separated_strings_from_input_line separated_strings;

  separated_strings = separate_input_line(input);
  if (separated_strings.error == YES) {
    return NULL;
  }

  assembler_AST *AST = (assembler_AST *)malloc(sizeof(assembler_AST));
  if (AST == NULL) {
    printf("%s\n", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }

  create_keywords(keywords);

  /* CHECKING IF THERE's A LABLE IN THE BEGINNING */

  substring_length = strlen(separated_strings.separated_strings[substring_number]);

  if (separated_strings.separated_strings[substring_number][substring_length - 1] == ':') {

    checker = is_a_valid_label(separated_strings.separated_strings[substring_number], keywords);

    if (checker == INVALID_LABEL) {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      return AST;
    }
    strcpy(AST->label, separated_strings.separated_strings[substring_number]);
    substring_number++;
    has_label_to_check_for_allowed_num_of_strings = YES;
  }

  /* CHECKING FOR DIRECTIVES */

  

  checker =
      parse_directive(separated_strings.separated_strings[substring_number]);
  switch (checker) {
  case DATA:

    substring_number++;
    for (i = substring_number, data_count = 0; i < separated_strings.number_of_strings; i++, data_count++) {
      if (isalpha(*separated_strings.separated_strings[i])) {
        checker = is_a_valid_label(separated_strings.separated_strings[i], keywords);
        if (checker == INVALID_LABEL) {
          AST->AST_type = AST_error;
          strcpy(AST->error, INVALID_LABEL_ERROR);
          return AST;
        }
        if (checker == VALID_LABEL) {
          AST->AST_options.directive.AST_directive_options.AST_directive_data.AST_directive_data_options = AST_directive_data_label;
        strcpy(AST->AST_options.directive.AST_directive_options.AST_directive_data.AST_directive_data_type[data_count].label, separated_strings.separated_strings[i]);
        continue;
        } else {
          AST->AST_type = AST_error;
          strcpy(AST->error, INVALID_LABEL_ERROR);
          return AST;
        }
      checker = is_a_valid_number(separated_strings.separated_strings[i], INT_MAX, INT_MIN, operand);
      if (checker == INVALID_NUMBER) {
        AST->AST_type = AST_error;
        strcpy(AST->error, INVALID_NUMBER_ERROR);
        return AST;
      }
      if (checker == VALID_NUMBER_TOO_BIG_OR_TOO_SMALL) {
        AST->AST_type = AST_error;
        strcpy(AST->error, NUMBER_OUT_OF_BOUNDS_ERROR);
        return AST;
      }
      AST->AST_options.directive.AST_directive_options.AST_directive_data.AST_directive_data_options = AST_directive_data_constant;
      AST->AST_options.directive.AST_directive_options.AST_directive_data.AST_directive_data_type[data_count].constant = *operand;
      }
    }
    AST->AST_type = AST_directive;
    AST->AST_options.directive.AST_directive_type = AST_directive_data;
    return AST;
  case STRING:
    if (*(separated_strings.separated_strings[substring_number + 1]) != '"') {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_STRING_ERROR);
      return AST;
    }
    if (*(separated_strings.separated_strings[substring_number + 1] + strlen(separated_strings.separated_strings[substring_number + 1]) - 1) != '"') {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_STRING_ERROR);
      return AST;
    }

    /* CHECK FOR EXCESS STRINGS */

    checker = check_valid_end_of_line(substring_number, separated_strings.number_of_strings -1 );
    if (checker == YES) {
    strncpy(AST->AST_options.directive.AST_directive_options.string, separated_strings.separated_strings[substring_number + 1] + 1, strlen(separated_strings.separated_strings[substring_number + 1]) - 2);
    AST->AST_type = AST_directive;
    AST->AST_options.directive.AST_directive_type = AST_directive_string;
    return AST;
    } else {
      AST->AST_type = AST_error;
      strcpy(AST->error, EXCESS_STRINGS_ERROR);
      return AST;
    }
  case ENTRY:
    checker = is_a_valid_label(separated_strings.separated_strings[substring_number + 1], keywords);
    if (checker == INVALID_LABEL) {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      return AST;
    }
    checker = check_valid_end_of_line(substring_number, separated_strings.number_of_strings - 1);
    if (checker == NO) {
      AST->AST_type = AST_error;
      strcpy(AST->error, EXCESS_STRINGS_ERROR);
      return AST;
    }
    AST->AST_type = AST_directive;
    AST->AST_options.directive.AST_directive_type = AST_directive_entry;
    strcpy(AST->AST_options.directive.AST_directive_options.label, separated_strings.separated_strings[substring_number + 1]);
    return AST;
  case EXTERN:
    checker = is_a_valid_label(
        separated_strings.separated_strings[substring_number + 1], keywords);
    if (checker == INVALID_LABEL) {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      return AST;
    }
    checker = check_valid_end_of_line(substring_number, separated_strings.number_of_strings - 1);
    if (checker == NO) {
      AST->AST_type = AST_error;
      strcpy(AST->error, EXCESS_STRINGS_ERROR);
      return AST;
    }
    AST->AST_type = AST_directive;
    AST->AST_options.directive.AST_directive_type = AST_directive_extern;
    strcpy(AST->AST_options.directive.AST_directive_options.label, separated_strings.separated_strings[substring_number + 1]);
    return AST;
  case DEFINE:
    if (separated_strings.number_of_strings != NUMBER_OF_VALID_OPERANDS_FOR_DEFINE) {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_DEFINE_ERROR);
      return AST;
    }

    substring_number++;
    checker = is_a_valid_label(separated_strings.separated_strings[substring_number], keywords);
    if (checker == INVALID_LABEL) {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_LABEL_ERROR);
      return AST;
    }
    strcpy(define_name, separated_strings.separated_strings[substring_number]);
    substring_number++;
    if (strcmp(separated_strings.separated_strings[substring_number], "=") != 0) {
      AST->AST_type = AST_error;
      strcpy(AST->error, INVALID_DEFINE_ERROR);
      return AST;
    }
    substring_number++;

    temp_string = separated_strings.separated_strings[substring_number];
    while (*temp_string != '\0') {
      if (!isdigit(*temp_string)) {
        AST->AST_type = AST_error;
        strcpy(AST->error, INVALID_DEFINE_ERROR);
        return AST;
      }
      temp_string++;
    }
    AST->AST_type = AST_directive;
    AST->AST_options.define.number = (int)strtol(separated_strings.separated_strings[substring_number], NULL, BASE_TEN);
    return AST;
  default:
    break;
  }

  /* CHECKING FOR INSTRUCTIONS */
  strcpy(temp_string, separated_strings.separated_strings[substring_number]);

  if (strcmp(temp_string, "mov") == 0 || strcmp(temp_string, "cmp") == 0 || 
      strcmp(temp_string, "add") == 0 || strcmp(temp_string, "sub") == 0 ||
      strcmp(temp_string, "lea") == 0) {

        /* TWO OPERAND INSTRUCTIONS */

        for (i = 0, num_of_commas = 0; i < separated_strings.number_of_strings; i++) {
          if (strcmp(separated_strings.separated_strings[i], ",") == 0) {
            num_of_commas++;
          }
        }
        if (num_of_commas == 0) {
          AST->AST_type = AST_error;
          strcpy(AST->error, MISSING_COMMA_ERROR);
          return AST;
        }
        if (num_of_commas > 1) {
          AST->AST_type = AST_error;
          strcpy(AST->error, EXCESS_COMMA_ERROR);
          return AST;
        }

        if (separated_strings.number_of_strings != NUMBER_OF_VALID_OPERANDS_FOR_TWO_OPERAND_INSTRUCTIONS + has_label_to_check_for_allowed_num_of_strings) {
          AST->AST_type = AST_error;
          strcpy(AST->error, INVALID_INSTRUCTION_ERROR);
          return AST;
        }

        substring_number++;

        if (strcmp(temp_string, "mov") == 0)
          opcode = AST_opcode_mov;
        if (strcmp(temp_string, "cmp") == 0)
          opcode = AST_opcode_cmp;
        if (strcmp(temp_string, "add") == 0)
          opcode = AST_opcode_add;
        if (strcmp(temp_string, "sub") == 0)
          opcode = AST_opcode_sub;
        if (strcmp(temp_string, "lea") == 0)
          opcode = AST_opcode_lea;

        AST->AST_type = AST_instruction;
          AST->AST_options.instruction.AST_instruction_option = opcode;
          checker = parse_operand(
              separated_strings.separated_strings[substring_number],
              opcode, operand_source, AST, keywords);
          if (checker == INVALID_OPERAND) {
            AST->AST_type = AST_error;
            strcpy(AST->error, INVALID_OPERAND_ERROR);
          }
          if (check_allowed_operand(checker, opcode, operand_source) == NO) {
            AST->AST_type = AST_error;
            strcpy(AST->error, INVALID_OPERAND_ERROR);
          }
          substring_number++;
          if (strcmp(separated_strings.separated_strings[substring_number], ",") != 0) {
            AST->AST_type = AST_error;
            strcpy(AST->error, MISSING_COMMA_ERROR);
            return AST;
          }
          substring_number++;
          checker = parse_operand(
              separated_strings.separated_strings[substring_number],
              opcode, operand_destination, AST, keywords);
          if (checker == INVALID_OPERAND) {
            AST->AST_type = AST_error;
            strcpy(AST->error, INVALID_OPERAND_ERROR);
          }
          if (check_allowed_operand(checker, opcode, operand_destination) == NO) {
            AST->AST_type = AST_error;
            strcpy(AST->error, INVALID_OPERAND_ERROR);
          }
          return AST;
        }

        if (strcmp(temp_string, "not") == 0 ||
            strcmp(temp_string, "clr") == 0 ||
            strcmp(temp_string, "inc") == 0 ||
            strcmp(temp_string, "dec") == 0 ||
            strcmp(temp_string, "jmp") == 0 ||
            strcmp(temp_string, "bne") == 0 ||
            strcmp(temp_string, "bne") == 0 ||
            strcmp(temp_string, "red") == 0 ||
            strcmp(temp_string, "prn") == 0 ||
            strcmp(temp_string, "jsr") == 0) {

          /* ONE OPERAND INSTRUCTIONS */

          for (i = 0; i < separated_strings.number_of_strings; i++) {
            if (strcmp(separated_strings.separated_strings[i], ",") == 0) {
              AST->AST_type = AST_error;
              strcpy(AST->error, EXCESS_COMMA_ERROR);
              return AST;
            }
          }
        
          if (separated_strings.number_of_strings != NUMBER_OF_VALID_OPERANDS_FOR_ONE_OPERAND_INSTRUCTIONS + has_label_to_check_for_allowed_num_of_strings) {
            AST->AST_type = AST_error;
            strcpy(AST->error, INVALID_INSTRUCTION_ERROR);
            return AST;
          }

          if (strcmp(temp_string, "not") == 0)
            opcode = AST_opcode_not;
          if (strcmp(temp_string, "clr") == 0)
            opcode = AST_opcode_clr;
          if (strcmp(temp_string, "inc") == 0)
            opcode = AST_opcode_inc;
          if (strcmp(temp_string, "dec") == 0)
            opcode = AST_opcode_dec;
          if (strcmp(temp_string, "jmp") == 0)
            opcode = AST_opcode_jmp;
          if (strcmp(temp_string, "bne") == 0)
            opcode = AST_opcode_bne;
          if (strcmp(temp_string, "red") == 0)
            opcode = AST_opcode_red;
          if (strcmp(temp_string, "prn") == 0)
            opcode = AST_opcode_prn;
          if (strcmp(temp_string, "jsr") == 0)
            opcode = AST_opcode_jsr;









        }
}



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
      check = does_label_have_only_digits(temp_label);
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

int does_label_have_only_digits(char *input) {
  while (*input != '\0') {
    if (!isdigit(*input)) {
      return NO;
    }
    input++;
  }
  return YES;
}


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

int parse_directive(char *input) {

  if (strcmp(input, ".data") == 0)
    return DATA;

 if (strcmp(input, ".string") == 0)
    return STRING;

 if (strcmp(input, ".entry") == 0)
    return ENTRY;

 if (strcmp(input, ".extern") == 0)
    return EXTERN;

 if (strcmp(input, ".define") == 0)
    return DEFINE;

  return INVALID_DIRECTIVE;
}

int check_valid_end_of_line(int number_of_strings, int substring_number) {
  if (number_of_strings != substring_number) {
    return NO;
  }
  return YES;
}


int check_allowed_operand(int operand_code, int source_destination, int instruction_opcode) {
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

  if (source_destination == operand_source) {
    if (strchr(instuction_operand_table[instruction_opcode].allowed_source, operand_code) == NULL) {
      return NO;
    } else {
      return YES;
    }
  }

  if (source_destination == operand_destination) {
    if (strchr(instuction_operand_table[instruction_opcode].allowed_destination, operand_code) == NULL) {
      return NO;
    } else {
      return YES;
    }
  }
  return NO;
}