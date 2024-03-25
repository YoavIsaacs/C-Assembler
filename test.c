#include <string.h>
#include "Frontend/assemblerAST.h"
#include "global/definitions.h"

int main(int argc, char *argv[]) {

  FILE *file;
  int i;
  char line[100];
  separated_strings_from_input_line s;
  file = fopen("test1.as", "r");
  if (file == NULL) {
    printf("Error: File not found\n");
    return 1;
  }
  
  while (fgets(line, sizeof(line), file)) {
    s = separate_input_line(line);
    for (i = 0; i < s.number_of_strings; i++) {
      printf("%s\n", s.separated_strings[i]);
    }
    for (i = 0; i < s.number_of_strings; i++) {
      s.separated_strings[i] = NULL;
    }
    s.number_of_strings = 0;
  }
  
  return 0;
}