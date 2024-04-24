#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataStructures/trie.h"
#include "Preprocessor/preprocessor.h"
#include "global/definitions.h"
#include "Middle/firstPass.h"
#include "Middle/secondPass.h"
#include "Backend/code_output.h"

int main(int argc, char *argv[]) {

  int i;
  char *file_name;
  FILE *current_file;
  translation_unit *tr; 

  tr = (translation_unit *)malloc(sizeof(translation_unit));

  for (i = 1; i < argc; i++) {
    file_name = preprocessor(argv[i]);
    memset(tr, 0, sizeof(translation_unit));
    if (file_name) {
      printf("Assembling file %s\n", file_name);
      current_file = fopen(file_name, "r");
      printf("File %s was opened\n", file_name);
      if (first_pass(tr, file_name, current_file) == NO) {
        printf("First pass was successful\n");
        rewind(current_file);
        if (second_pass(tr, file_name, current_file) == NO) {
          printf("Second pass was successful\n");
          code_output(tr, argv[i]);
          printf("File %s was successfully assembled\n", file_name);
        }
      }
    }
  }
  free(tr);



  return 0;
}