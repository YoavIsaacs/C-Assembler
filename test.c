#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Preprocessor/preprocessor.h"
#include "global/definitions.h"
#include "Middle/firstPass.h"

int main(int argc, char *argv[]) {

  int check;
  FILE *am_file;
  char *filename;
  translation_unit *tr;

  filename = (char *)malloc(strlen(argv[1]) + 5 + 11);
  if (filename == NULL) {
    printf("Error: Memory allocation failed\n");
    exit(1);
  }

  strcpy(filename, "build/bin/");
  strcat(filename, preprocessor(argv[1]));

  printf("Preprocessed file: %s.\n", filename);
  
  am_file = fopen("/mnt/d/University/2024/2024A/C/Maman 14/Maman14/build/bin/test1.am", "r"); 
  if (am_file == NULL) {
    printf("Error: File %s not found\n", filename);
    exit(1);
  }

  tr = (translation_unit*)malloc(sizeof(translation_unit));
  if (tr == NULL) {
    printf("%s", MEMORY_ALLOCATION_ERROR);
    exit(1);
  }
  check = first_pass(tr, filename, am_file);

  if (check == YES) {
    printf("First pass completed successfully\n");
  } else {
    printf("First pass failed\n");
  }



  return 0;
}