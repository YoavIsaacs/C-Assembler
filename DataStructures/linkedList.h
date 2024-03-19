#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "../global/definitions.h"

List* create_list();
void add_to_list(List*, Macro*);
void* remove_from_list(List*);
Macro* get_from_list(List*, char*);
void free_list(List*);



#endif