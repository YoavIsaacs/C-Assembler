#ifndef _ASSEMBLER_AST
#define _ASSEMBLER_AST

#include "../global/definitions.h"
#include "../Preprocessor/preprocessor.h"
#include <stdio.h>

assembler_AST* parse(char *, char *);
assembler_AST* parse_two_operands(assembler_AST *, char *, char *, char *);
assembler_AST* parse_one_operand(assembler_AST *, char *, char *, char *);
assembler_AST* parse_no_operands(assembler_AST *, char *, char *, char *);
assembler_AST* parse_data(assembler_AST *, char *, char *, char *);
assembler_AST* parse_string(assembler_AST *, char *, char *, char *);
assembler_AST* parse_entry_extern(assembler_AST *, char *, char *, char *);
assembler_AST* parse_label(assembler_AST *, char *, char *, char *);
int check_two_operands(char *);
int check_one_operand(char *);
int check_no_operands(char *);

#endif
