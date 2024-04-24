#ifndef FIRSTPASS_H
#define FIRSTPASS_H


#include "../global/definitions.h"
#include "../FrontEnd/assemblerAST.h"


int first_pass(translation_unit *, const char *, FILE *);
int look_for_last_entry(symbol_entry_node *);
void increment_IC(symbol_entry_node *, int); 
void point_to_entries(symbol_entry_node *, symbol*, int* );


#endif