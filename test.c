#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataStructures/hashTable.h"
#include "Preprocessor/preprocessor.h"
#include "global/definitions.h"
#include "Middle/firstPass.h"

int main(int argc, char *argv[]) {

  int check;
  FILE *am_file;
  char *filename;
  hashTable *symbol_table;

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

  symbol_table = create_hash_table();
  check = firstPass(symbol_table, filename, am_file);

  if (check == YES) {
    printf("First pass completed successfully\n");
  } else {
    printf("First pass failed\n");
  }



  return 0;
}