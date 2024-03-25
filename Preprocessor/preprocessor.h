#ifndef _PREPROCESSOR_H_
#define _PREPROCESSOR_H_

#include "../DataStructures/macro.h"
#include "../DataStructures/linkedList.h"
#include "../DataStructures/trie.h"
#include <stdio.h>

char *preprocessor(char *);
int checkLine(char *, TrieNode *);
int checkLegalName(char *);
void skipSpaces(char **);
void constructor(const void *);
void destructor(void *);
int findWordLength(char *);
void printMacro(char *, FILE *, List *);
int lineEndsLegally(char *);

#endif
