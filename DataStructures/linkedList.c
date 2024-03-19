#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

List* create_list() {
    List* new_list = (List*)malloc(sizeof(List));
    new_list->size = 0;
    new_list->head = NULL;
    return new_list;
}
 
void add_to_list(List* list, Macro* data) {
    Node* new_node;
    Macro *macro;
    new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }
    macro = (Macro*)malloc(sizeof(Macro));
    if (macro == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }
    memcpy(macro, data, sizeof(Macro));
    new_node->data = macro;
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}
 
void* remove_from_list(List* list) {
    Node *node_to_remove;
    void* data;
    if (list->size == 0) {
        return NULL;
    }
    node_to_remove = list->head;
    data = node_to_remove->data;
    list->head = node_to_remove->next;
    free(node_to_remove);
    list->size--;
    return data;
}
 
Macro *get_from_list(List* list, char* name) {
    Node* currentNode = list->head;
    Macro* macro;
    while (currentNode != NULL) {
        macro = (Macro*)currentNode->data;
        if (strcmp(macro->name, name) == 0) {
            return macro;
        }
        currentNode = currentNode->next;
    }
    return NULL;
}

void free_list(List* list) {
    Node* current_node = list->head;
    while (current_node != NULL) {
        Node* next_node = current_node->next;
        free(current_node->data);
        free(current_node);
        current_node = next_node;
    }
    free(list);
}