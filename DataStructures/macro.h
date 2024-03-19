#ifndef _MACRO_H_
#define _MACRO_H_

#include "../global/definitions.h"

void createMacro(Macro *,char *, char **, int);
void addMacro(Macro *, char *);
void deleteMacro(Macro *);
void emptyMacro(Macro *macro);


#endif
