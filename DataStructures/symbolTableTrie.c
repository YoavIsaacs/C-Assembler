#include "../global/definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include "symbolTableTrie.h"


symbol_entry_node *create_symbol_table_node() {
    int i;
    symbol_entry_node *node = (symbol_entry_node *)malloc(sizeof(symbol_entry_node));
    if (node == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(1);
    }
    
    for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
        node->next[i] = NULL;
    }
    node->is_symbol = NO;
    node->symbol_data = NULL;

    return node;
}


void insert_to_symbol_table(symbol_entry_node *node, symbol *data) {
    int i;
    int index;
    symbol_entry_node *temp_node = node;

    for (i = 0; data->name[i] != '\0'; i++) {
        index = data->name[i] - TRIE_BASE_CHAR;
        if (temp_node->next[index] == NULL) {
            temp_node->next[index] = create_symbol_table_node();
        }
        temp_node = temp_node->next[index];
    }
    temp_node->symbol_data = data;
    temp_node->is_symbol = YES;
}


symbol *search_symbol_table(symbol_entry_node *node, char *name) {
    int i;
    int index;

    symbol_entry_node *temp_node = node;

    for (i = 0; name[i] != '\0'; i++) {
        index = name[i] - TRIE_BASE_CHAR;
        if (temp_node->next[index] == NULL) {
            return NULL;
        }
        temp_node = temp_node->next[index];
    }
    if (temp_node != NULL && temp_node->is_symbol == YES) {
        return temp_node->symbol_data;
    }
    return NULL;
}


void delete_symbol_table(symbol_entry_node *node) {
    int i;
    if (node == NULL) {
        return;
    }
    
    for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
        if (node->next[i] != NULL) {
            delete_symbol_table(node->next[i]);
        }
    }
    free(node->symbol_data);
    free(node);
}


void print_symbol_table_names(symbol_entry_node* node) {
    int i;
    if (node == NULL) {
        return;
    }
    
    for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
        if (node->next[i] != NULL) {
            if (node->next[i]->is_symbol == YES) {
                printf("%s at address %d\n", node->next[i]->symbol_data->name, node->next[i]->symbol_data->address);
            }
            print_symbol_table_names(node->next[i]);
        }
    }
    
}
