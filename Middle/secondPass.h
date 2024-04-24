#include "../global/definitions.h"
#include <stdio.h>

int second_pass(translation_unit *, char *, FILE *);
void add_to_code_and_data_and_external_image(int **, int *);
int search_externals(external_table*, int, char *);
int code_labels(symbol* , translation_unit *, assembler_AST *, int, int);
int find_external_index(external_table *, int , char *);