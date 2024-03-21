#include "assemblerAST.h"
#include "../global/definitions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

assembler_AST *parse(char *input, char *file_name) {
  char *line;
  char *current_word;
  char temp[TEMP_INPUT_LENGTH];
  int i;
  int word_length;
  int line_length;
  assembler_AST *AST;

  AST = (assembler_AST *)malloc(sizeof(assembler_AST));
  if (AST == NULL) {
    printf("%s", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }
  AST->has_error = NO;
  line_length = strlen(temp);
  if (line_length > MAX_LINE_LENGTH) {
    AST->has_error = YES;
    strcpy(AST->error, LINE_TOO_LONG_ERROR);
    return AST;
  }
  line = (char *)malloc(sizeof(char) * strlen(temp) + 1);
  if (line == NULL) {
    printf("%s", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }
  strcpy(line, temp);
  skipSpaces(&line);
  word_length = findWordLength(line);
  current_word = (char *)malloc((sizeof(char) * word_length) + 1);
  if (current_word == NULL) {
    printf("%s", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }
  strncpy(current_word, line, word_length);
  line += word_length + 1;

  if (check_two_operands(current_word) == YES)
    return parse_two_operands(AST, line, current_word, file_name);

  if (check_one_operand(current_word) == YES)
    return parse_one_operand(AST, line, current_word, file_name);

  if (check_no_operands(current_word) == YES)
    return parse_no_operands(AST, line, current_word, file_name);

  if (strcmp(current_word, ".data"))
    return parse_data(AST, line, current_word, file_name);

  if (strcmp(current_word, ".string"))
    return parse_string(AST, line, current_word, file_name);

  if (strcmp(current_word, ".entry") || strcmp(current_word, ".extern"))
    return parse_entry_extern(AST, line, current_word, file_name);

  return parse_label(AST, line, current_word, file_name);
  return AST;
}

int check_two_operands(char *word) {
  if (strcmp(word, "mov") == 0 || strcmp(word, "cmp") == 0 ||
      strcmp(word, "add") == 0 || strcmp(word, "sub") == 0 ||
      strcmp(word, "lea") == 0)
    return YES;

  return NO;
}

int check_one_operand(char *word) {

  if (strcmp(word, "not") == 0 || strcmp(word, "clr") == 0 ||
      strcmp(word, "inc") == 0 || strcmp(word, "dec") == 0 ||
      strcmp(word, "jmp") == 0 || strcmp(word, "bne") == 0 ||
      strcmp(word, "red") == 0 || strcmp(word, "prn") == 0 ||
      strcmp(word, "jsr") == 0)
    return YES;

  return NO;
}

int check_no_operands(char *word) {
  if (strcmp(word, "rts") == 0 || strcmp(word, "hlt") == 0)
    return YES;

  return NO;
}

assembler_AST *parse_label(assembler_AST *AST, char *line, char *called_word,
                           char *file_name) {
  char temp[TEMP_INPUT_LENGTH];
  char *current_word;
  int word_length;

  if (!isalpha(*called_word)) {
    AST->has_error = YES;
    strcpy(AST->error, ILLEGAL_LABEL_NAME_ERROR);
    return AST;
  }
  word_length = strlen(called_word);
  if (word_length > MAX_LABEL_LENGTH) {
    AST->has_error = YES;
    strcpy(AST->error, LABEL_TOO_LONG_ERROR);
  }
  strncpy(AST->label_name, called_word, word_length);
  line += word_length + 1;
  skipSpaces(&line);
  word_length = findWordLength(line);
  current_word = (char *)malloc((sizeof(char) * word_length) + 1);
  if (current_word == NULL) {
    printf("%s", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }
  strncpy(current_word, line, word_length);
  line += word_length + 1;

  if (check_two_operands(current_word) == YES)
    return parse_two_operands(AST, line, current_word, file_name);

  if (check_one_operand(current_word) == YES)
    return parse_one_operand(AST, line, current_word, file_name);

  if (check_no_operands(current_word) == YES)
    return parse_no_operands(AST, line, current_word, file_name);

  if (strcmp(current_word, ".data"))
    return parse_data(AST, line, current_word, file_name);

  if (strcmp(current_word, ".string"))
    return parse_string(AST, line, current_word, file_name);

  if (strcmp(current_word, ".entry") || strcmp(current_word, ".extern"))
    return parse_entry_extern(AST, line, current_word, file_name);

  AST->has_error = YES;
  strcpy(AST->error, UNKNOWN_COMMAND_ERROR);
  return AST;
}

assembler_AST *parse_data(assembler_AST *AST, char *line, char *current_word,
                          char *file_name){
  int num_length;

  if (check_nums(line, AST) == NO)
    return AST;
  
  AST->AST_type.directive.directive_operand.data.data_count = 0;
  while (AST->AST_type.directive.directive_operand.data.data_count < MAX_DATA_SIZE && (*line) != '\0') {
    num_length = findWordLength(line);
    if (num_length > MAX_INT_SIZE || (*line == '-' && num_length > MAX_INT_SIZE + 1) || (*line == '+' && num_length > MAX_INT_SIZE + 1)) {
      AST->has_error = YES;
      strcpy(AST->error, NUMBER_TOO_LONG_ERROR);
      return AST;
    }
    AST->AST_type.directive.directive_operand.data.data[AST->AST_type.directive.directive_operand.data.data_count++] = (int)strtol(line, &line, BASE_TEN);
    skipSpaces(&line);
  }
  strcpy(AST->label_name, current_word);
}

assembler_AST *parse_string(assembler_AST *AST, char *line, char *current_word,
                          char *file_name){

  char temp[MAX_LINE_LENGTH];
  char *string;
  int word_length;

  skipSpaces(&line);
  word_length = findWordLength(line);
  if (*line != '\"' && (*(line + word_length)) != '\"') {
    AST->has_error = YES;
    strcpy(AST->error, ILLEGAL_STRING_ERROR);
    return AST;
  }
  AST->AST_type.directive.directive_operand.string = (char*)malloc(word_length + 1); 
  if (AST->AST_type.directive.directive_operand.string == NULL) {
    printf("%s", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }
  strncpy(AST->AST_type.directive.directive_operand.string, temp, word_length);
  return AST;
} 

assembler_AST *parse_entry_extern(assembler_AST *AST, char *line, char *current_word,
                          char *file_name){

  char temp[MAX_LINE_LENGTH];
  int word_length;

  skipSpaces(&line);
  if (!isalpha(*line)) {
    AST->has_error = YES;
    strcpy(AST->error, ILLEGAL_LABEL_NAME_ERROR);
    return AST;
  }
  word_length = findWordLength(line);
  



}


int check_nums(char *input, assembler_AST *AST) {
  int is_decimal = NO, has_comma = NO, first_digit = NO;
  while (input && *input != '\0') {
    if (isspace(*input)) {
      input++;
      continue;
    }
    if (!isdigit(*input) && !isspace(*input) && *input != ',' &&
        *input != '-' && *input != '+' && *input != '\0' && *input != '\n' &&
        *input != '\005') {
      AST->has_error = YES;
      strcpy(AST->error, INVALID_NUMBER_ERROR);
      return NO;
    }
    if (*input == ',' && (lineEndsLegally(input)) == YES) {
      AST->has_error = YES;
      strcpy(AST->error, INVALID_END_OF_LINE_ERROR);
      return NO;
    }
    if (*input == ',' && has_comma == NO) {
      has_comma = YES;
    } else if (*input == ',' && has_comma == YES) {
      AST->has_error = YES;
      strcpy(AST->error, DOUBLE_COMMA_ERROR);
      return NO;
    }
    if (*input == '\0' && first_digit == YES) {
      AST->has_error = YES;
      strcpy(AST->error, INVALID_NUMBER_ERROR);
      return NO;
    }
    if (*input == '-' && first_digit == NO) {
      AST->has_error = YES;
      strcpy(AST->error, INVALID_NUMBER_ERROR);
      return NO;
    }
    if (isdigit(*input && has_comma == NO && first_digit == NO)) {
      AST->has_error = YES;
      strcpy(AST->error, MISSING_COMMA_ERROR);
      return NO;
    }

    if (*input == '+' && first_digit == YES) {
      input++;
    }

    if (*input == ',') {
      has_comma = YES;
      first_digit = YES;
      is_decimal = NO;
    }
    if (has_comma == YES && isdigit(*input)) {
      has_comma = NO;
    }

    if (first_digit == NO && *input == '.') {
      is_decimal = YES;
    }
    if (isdigit(*input) && first_digit == YES) {
      first_digit = NO;
    }
    if (*input == '\0' && has_comma == YES) {
      AST->has_error = YES;
      strcpy(AST->error, INVALID_END_OF_LINE_ERROR);
      return NO;
    }
    input++;
  }
  return YES;
}