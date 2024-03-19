#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void createMacro(Macro *macro, char *name, char **lines, int numOfLines) {
  macro->name = (char *)malloc(strlen(name) + 1);
  if (macro->name == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
  }
  strcpy(macro->name, name);

  macro->lines = NULL;
  macro->numOfLines = numOfLines;
  macro->error = NO;
}

void addMacro(Macro *macro, char *line) {
  macro->lines = (char **)realloc(macro->lines, (macro->numOfLines + 1) * sizeof(char *));
  if (macro->lines == NULL) {
    printf("Error: realloc failed\n");
    exit(1);
  }
  macro->lines[macro->numOfLines] = (char *)malloc(strlen(line) + 1);
  if (macro->lines[macro->numOfLines] == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
  }
  strcpy(macro->lines[macro->numOfLines], line);
  macro->numOfLines++;
}

void freeMacro(Macro *macro) {
  int i;
  for (i = 0; i < macro->numOfLines; i++) {
    free(macro->lines[i]);
  }
  free(macro->lines);
  free(macro->name);
}

void emptyMacro(Macro *macro) {
  macro->name = NULL;
  macro->lines = NULL;
  macro->numOfLines = 0;
  macro->error = NO;
}