/* Implementação da biblioteca list.h */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int insert(Node **pNode, Weather target) {

  // Se for uma posição vazia, já houver um número existente
  if(!(*pNode)) {
    // Aloca um novo endereço pra armazenar as novas informações e verifica
    //  se houve sucesso.
    Node *aux = (Node *) malloc(sizeof(Node));
    if (!aux) return 0;   // Caso falhe, retorna falha

    // Inicializa os campos com os valores pré determinados
    aux->next = NULL;
    aux->card = target;

    // Associa o novo espaço populado com a lista
    (*pNode) = aux;
    puts("Card created");
    return 1;   // Retorna sucesso
  }
  else if((*pNode)->card.numPeople == target.numPeople) {
      (*pNode)->card = target;
      return 1;
  }
  else{
    printf("Searching insertion place\n");
    return insert(&((*pNode)->next), target); // Continua procurando
  }
}

float findAdjacents(Node *node, float target) {

  puts("Searching value");
  static float temp = 0;

  if(!node) return temp;
  else if(node->card.numPeople > target) return temp;
  else {
    temp = node->card.temperature;
    return findAdjacents(node->next, target);
  }
}

void printList(Node *list) {
  if(list) {
    printf("Numero pessoas: %i, Temperatura: %.2f\n", list->card.numPeople, list->card.temperature);
    printList(list->next);
  }
}
