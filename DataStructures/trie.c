/**
 * @file trie.c
 * @author Yoav Isaacs (yoav.isaacs@gmail.com)
 * @brief This file contains the implementation of the trie data structure.
 * @version 1.1
 * @date 2024-02-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Create a Trie Node object
 * 
 * @return An empty TrieNode
 */
TrieNode *create_trie_node() {
  TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
  int i;
  if (node == NULL) {
    fprintf(stderr, "Error: malloc failed\n");
    exit(1);
  }
  for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
    node->next[i] = NULL;
  }
  node->name = NULL;
  node->isEndOfWord = NO;

  return node;
}

/**
 * @brief This function inserts a word into the trie data structure. If the word
  * is already in the trie, it will add the line number to the list of line numbers
  * for that word. If the word is not in the trie, it will add the word to the trie
  * and create a list of line numbers for that word.
 * 
 * @param root The root of the trie data structure.
 */
void insert_trie(TrieNode *root, char *currentName, const char *originalName) {
  int i;
  int index;
  TrieNode *tempNode = root;
  for (i = 0; currentName[i] != '\0'; i++) {
    index = currentName[i] - TRIE_BASE_CHAR;
    if (tempNode->next[index] == NULL) {
      tempNode->next[index] = create_trie_node();
    }
    tempNode = tempNode->next[index];
  }
  tempNode->name = (char *)malloc(strlen(originalName) + 1);
    if (tempNode->name == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(1);
    }
    strcpy(tempNode->name, originalName);
    tempNode->isEndOfWord = YES;
  }

/**
 * @brief This function searches the trie data structure for a word.
  * 
  * @param root The root of the trie data structure.
  * @param word The word to be searched for.
  * @return YES (1) if the word is found, NO (0) if the word is not found.
 */
int search_trie(TrieNode *root, char *word) {
  int i, index;
  TrieNode *tempNode = root;
  for (i = 0; word[i] != '\0'; i++) {
    index = word[i] - TRIE_BASE_CHAR;
    if (tempNode->next[index] == NULL) {
      return NO;
    }
    tempNode = tempNode->next[index];
  }
    if (tempNode != NULL && tempNode->isEndOfWord == YES) {
        return YES;
    }
    return NO;
}

/**
 * @brief This function deletes the trie data structure.
 * 
 * @param root The root of the trie data structure.
 */
void delete_trie(TrieNode *root) {
  int i;
  if (root == NULL) {
    return;
  }
  
  for (i = 0; i < POSSIBLE_CHARACTERS; i++) {
    if (root->next[i] != NULL) {
      delete_trie(root->next[i]);
    }
  }

  if (root->name != NULL) {
    free(root->name);
  }
  free(root);
}