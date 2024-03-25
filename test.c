#include <string.h>
#include "Frontend/assemblerAST.h"
#include "global/definitions.h"

int main(int argc, char *argv[]) {

  int i;
  separated_strings_from_input_line s;
  char temp[100] = "d s[   5   ] , , , st   hfjk[jhklr4hljk4rl                      ]";
  s = separate_input_line(temp);

    for (i = 0; i < s.number_of_strings; i++) {
      printf("%s\n", s.separated_strings[i]);
    }
  return 0;
}