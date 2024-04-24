#ifndef SYMBOL_TABLE_TRIE_H
#define SYMBOL_TABLE_TRIE_H

#include <stdio.h>
#include "../global/definitions.h"




symbol_entry_node *create_symbol_table_node();
void insert_to_symbol_table(symbol_entry_node*, symbol *);
symbol *search_symbol_table(symbol_entry_node*, char *);
void delete_symbol_table(symbol_entry_node*);
void print_symbol_table_names(symbol_entry_node*);

#endif
