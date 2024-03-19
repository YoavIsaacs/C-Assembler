/**
 * @file trie.h
 * @author Yoav Isaacs (yoav.isaacs@gmail.com)
 * @brief This file contains the function prototypes for the trie data structure
 * and the definition of the trie node structure.
 * @version 0.1
 * @date 2024-02-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include "../global/definitions.h"




TrieNode *create_trie_node();
void insert_trie(TrieNode *, char *, const char *);
int search_trie(TrieNode *, char *);
void print_trie(TrieNode *);
void delete_trie(TrieNode *);

#endif
