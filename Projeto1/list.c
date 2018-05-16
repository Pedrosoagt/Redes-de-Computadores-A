/* Implementação da biblioteca list.h */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int insert(Node *node, Status target) {

  Node *current = node;

  if(!current || current->state.numPeople == target.numPeople){

    Node *aux = (Node *) malloc(sizeof(Node));

    if (!aux) return 0;
    aux->next = NULL;
    aux->state = target;
    current = aux;
    return 1;
  }
  else return insert(current->next, target);
}

int find(Node *node, int target) {

  static int index = 0;

  if (!node) return -1;
  else if (*node->state.numPeople == target) return index;
  else {
    index++;
    return find(node, target);
  }
}
