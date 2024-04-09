#include "preprocessor.h"
#include "../DataStructures/linkedList.h"
#include "../DataStructures/trie.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *preprocessor(char *inputFileName) {
  FILE *originalFile;
  FILE *outputFile;
  char *outputFileName;
  char *fileName;
  char *line;
  char *macroName;
  char temp[MAX_BUFFER_LENGTH];
  int inMacro;
  int macroNameLength;
  Macro *currentMacro;
  int i;
  TrieNode *macroNames;
  List *macros;

  fileName = (char *)malloc(strlen(inputFileName) + 5);
  if (fileName == NULL) {
    printf("Error: Memory allocation failed\n");
    exit(1);
  }
  strcpy(fileName, inputFileName);
  strcat(fileName, ASSEMBLY_FILE_EXTENSION);

  outputFileName = (char *)malloc(strlen(inputFileName) + 5);
  if (outputFileName == NULL) {
    printf("Error: Memory allocation failed\n");
    exit(1);
  }
  strcpy(outputFileName, inputFileName);
  strcat(outputFileName, POST_PREPROCESSOR_FILE_EXTENSION);

  originalFile = fopen(fileName, "r");
  if (originalFile == NULL) {
    printf("Error: File %s not found\n", fileName);
    exit(1);
  }
  outputFile = fopen(outputFileName, "w");

  macros = create_list();
  macroNames = create_trie_node();
  currentMacro = (Macro *)malloc(sizeof(Macro));
  if (currentMacro == NULL) {
    printf("Error: Memory allocation failed\n");
    exit(1);
  }

  emptyMacro(currentMacro);

  inMacro = NO;
  fflush(stdout);

  while (fgets(temp, MAX_LINE_LENGTH, originalFile) != NULL) {
    line = malloc(strlen(temp) + 1);
    if (line == NULL) {
      printf("Error: Memory allocation failed\n");
      exit(1);
    }
    strcpy(line, temp);
    switch (checkLine(line, macroNames)) {
    case empty:
      break;
    case comment:
      break;
    case other:
      if (inMacro) {
        if (currentMacro->lines == NULL) {
          currentMacro->lines =
              (char **)malloc(LINE_INCEREMENTS * sizeof(char *));
          if (currentMacro->lines == NULL) {
            printf("Error: Memory allocation failed\n");
            exit(1);
          }
          currentMacro->numOfLines = 0;
        }
        if (currentMacro->numOfLines % LINE_INCEREMENTS == 0) {
          currentMacro->lines = realloc(
              currentMacro->lines,
              (currentMacro->numOfLines + LINE_INCEREMENTS) * sizeof(char *));
          if (currentMacro->lines == NULL) {
            printf("Error: Memory allocation failed\n");
            exit(1);
          }
        }
        currentMacro->lines[currentMacro->numOfLines] =
            malloc(strlen(line) + 1);
        if (currentMacro->lines[currentMacro->numOfLines] == NULL) {
          printf("Error: Memory allocation failed\n");
          exit(1);
        }
        strcpy(currentMacro->lines[currentMacro->numOfLines], line);
        currentMacro->numOfLines++;
      } else {
        fprintf(outputFile, "%s", line);
        fflush(outputFile);
      }
      break;
    case start:
      line += MACRO_START_LENGTH;
      skipSpaces(&line);
      macroNameLength = findWordLength(line);
      currentMacro->name = realloc(currentMacro->name, macroNameLength + 1);
      if (currentMacro->name == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
      }
      currentMacro->name[macroNameLength] = '\0';
      if (search_trie(macroNames, line)) {
        currentMacro->error = YES;
        break;
      }

      strncpy(currentMacro->name, line, macroNameLength);
      if (strcmp(currentMacro->name, "mcr") == 0 ||
          strcmp(currentMacro->name, "endmcr") == 0 ||
          strcmp(currentMacro->name, "cmp") == 0 ||
          strcmp(currentMacro->name, "mov") == 0 ||
          strcmp(currentMacro->name, "add") == 0 ||
          strcmp(currentMacro->name, "sub") == 0 ||
          strcmp(currentMacro->name, "not") == 0 ||
          strcmp(currentMacro->name, "clr") == 0 ||
          strcmp(currentMacro->name, "lea") == 0 ||
          strcmp(currentMacro->name, "inc") == 0 ||
          strcmp(currentMacro->name, "dec") == 0 ||
          strcmp(currentMacro->name, "jmp") == 0 ||
          strcmp(currentMacro->name, "bne") == 0 ||
          strcmp(currentMacro->name, "red") == 0 ||
          strcmp(currentMacro->name, "prn") == 0 ||
          strcmp(currentMacro->name, "jsr") == 0 ||
          strcmp(currentMacro->name, "rts") == 0 ||
          strcmp(currentMacro->name, "hlt") == 0) {
        currentMacro->error = YES;
        free(currentMacro->name);
        break;
      }

      line += macroNameLength + 1;
      if (lineEndsLegally(line) == NO) {
        fprintf(outputFile, "%s", temp);
        currentMacro->error = YES;
        free(currentMacro->name);
        break;
      }
      inMacro = YES;

      insert_trie(macroNames, currentMacro->name, currentMacro->name);
      currentMacro->numOfLines = 0;
      break;
    case stop:
      inMacro = NO;
      add_to_list(macros, currentMacro);
      emptyMacro(currentMacro);
      break;
    case error:
      break;
    case call:
      macroNameLength = findWordLength(line);
      macroName = (char *)malloc(macroNameLength + 1);
      if (macroName == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
      }
      strncpy(macroName, line, macroNameLength);
      macroName[macroNameLength] = '\0';
      currentMacro = get_from_list(macros, macroName);
      for (i = 0; i < currentMacro->numOfLines; i++) {
        fprintf(outputFile, "%s", currentMacro->lines[i]);
        fflush(outputFile);
      }

      break;
    }
  }
  free(currentMacro->name);
  free(currentMacro);

  return outputFileName;
}

int checkLine(char *line, TrieNode *trie) {
  char *currentWord;
  int i = 0;

  if (strlen(line) > MAX_LINE_LENGTH) {
    return error;
  }

  skipSpaces(&line);
  if (*line == ';') {
    return comment;
  }
  if (*line == '\0' || *line == '\n') {
    return empty;
  }

  i = findWordLength(line);

  currentWord = (char *)malloc(i + 1);
  if (currentWord == NULL) {
    printf("Error: Memory allocation failed\n");
    exit(1);
  }

  currentWord[i] = '\0';

  strncpy(currentWord, line, i);

  if (strcmp(currentWord, "mcr") == 0) {
    return start;
  }

  if (strcmp(currentWord, "endmcr") == 0) {
    return stop;
  }

  if (search_trie(trie, currentWord)) {
    return call;
  }

  return other;
}

void skipSpaces(char **line) {
  while (**line != '\0') {
    if (**line != ' ' || **line != '\t') {
      return;
    }
    (*line)++;
  }
}

int findWordLength(char *line) {
  int length = 0;
  char *currentChar = line;

  if (*currentChar == '.') {
    currentChar++;
    length++;
  }
  while (*currentChar != '\0' && *currentChar != ' ' && *currentChar != '\t' &&
         *currentChar != ',' && *currentChar != '.' && *currentChar != '\n') {
    length++;
    currentChar++;
  }
  return length;
}

int lineEndsLegally(char *line) {
  int i = 0;
  while (line[i] != '\0' && line[i] != '\n') {
    if (!isspace(*line)) {
      return NO;
    }
    i++;
  }
  return YES;
}