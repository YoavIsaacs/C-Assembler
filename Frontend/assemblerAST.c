#include "../global/definitions.h"
#include <stdlib.h>
#include <string.h>
#include "assemblerAST.h"


assembler_AST* parse(FILE *input, char *file_name) {
    char* line;
    char* current_word;
    char temp[TEMP_INPUT_LENGTH];
    int i;
    int word_length;
    int line_length;
    assembler_AST* AST;
    
    while(fgets(temp, MAX_LINE_LENGTH, input) != NULL) {
        AST = (assembler_AST*)malloc(sizeof(assembler_AST));
        line_length = strlen(temp);
        if(line_length > MAX_LINE_LENGTH) {
            AST->AST_type.instruction.possible_command = err;
            strcpy(AST->error, LINE_TOO_LONG_ERROR);
            return AST;
        }
        line = (char*)malloc(sizeof(char) * strlen(temp) + 1);
        if(line == NULL) {
            printf("%s", MEMORY_ALLOCATION_ERROR);
            exit(1);
        }
        strcpy(line, temp);
        skipSpaces(&line);
        word_length = findWordLength(line);
        current_word = (char*)malloc((sizeof(char) * word_length) + 1);
        if(current_word == NULL) {
            printf("%s", MEMORY_ALLOCATION_ERROR);
            exit(1);
        }
        strncpy(current_word, line, word_length);
        line += word_length + 1;

        if (check_two_operands(current_word) == YES)
          return parse_two_operands(AST, line, current_word, file_name);
        
        if (check_one_operand(current_word) == YES)
          return parse_one_operand(AST, line, current_word ,file_name);

        if (check_no_operands(current_word) == YES)
          return parse_no_operands(AST, line, current_word ,file_name);

        if (strcmp(current_word, ".data"))
            return parse_data(AST, line, current_word ,file_name);

        if (strcmp(current_word, ".string"))
            return parse_string(AST, line, current_word ,file_name);

        if (strcmp(current_word, ".entry") ||
            strcmp(current_word, ".extern"))
            return parse_entry_extern(AST, line, current_word ,file_name);
        
        return parse_label(AST, line, current_word ,file_name);


    }
    return AST;
}




int check_two_operands(char* word) {
    if (strcmp(word, "mov") == 0 || strcmp(word, "cmp") == 0 ||
        strcmp(word, "add") == 0 || strcmp(word, "sub") == 0 ||
        strcmp(word, "lea") == 0)
        return YES;
    
    return NO;
}



int check_one_operand(char* word) {

  if (strcmp(word, "not") == 0 || strcmp(word, "clr") == 0 ||
      strcmp(word, "inc") == 0 || strcmp(word, "dec") == 0 ||
      strcmp(word, "jmp") == 0 || strcmp(word, "bne") == 0 ||
      strcmp(word, "red") == 0 || strcmp(word, "prn") == 0 ||
      strcmp(word, "jsr") == 0)
        return YES;
    
    return NO;
}


int check_no_operands(char* word) {
    if (strcmp(word, "rts") == 0 || strcmp(word, "hlt") == 0)
        return YES;
    
    return NO;
}


assembler_AST* parse_label(assembler_AST *AST, char *line, char *called_word, char *file_name) {
    char temp[TEMP_INPUT_LENGTH];
    char* current_word;
    int word_length;

    word_length = strlen(called_word);
    if (word_length > MAX_LABEL_LENGTH) {
        AST->AST_type.instruction.operands = err;
    }

}