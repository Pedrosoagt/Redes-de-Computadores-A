/* Implementação da biblioteca list.h */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*
int insert(Node *node, Weather target) {

  // Resgata node pra ser manipulado mais facilmente
  Node *current = node;

  // Se for uma posição vazia, já houver um número existente
  if(!current){
    // Aloca um novo endereço pra armazenar as novas informações e verifica
    //  se houve sucesso.
    Node *aux = NULL;
    aux = (Node *) malloc(sizeof(Node));
    if (!aux) return 0;   // Caso falhe, retorna falha

    // Inicializa os campos com os valores pré determinados
    aux->next = NULL;
    aux->card = target;

    // Associa o novo espaço populado com a lista
    current = aux;
    printf("Num: %i, Temp: %g\n", current->card.numPeople, current->card.temperature);
    return 1;   // Retorna sucesso
  }
  else if(current->card.numPeople == target.numPeople){
    current->card = target;
    return 1;
  }
  else return insert(current->next, target); // Continua procurando
}
*/


int insert(Node **pNode, Weather target) {

  puts("Begin insert");
  printf("pNode: %li\n", (*pNode));

  Node *current = (*pNode);
  // Se for uma posição vazia, já houver um número existente
  if(!current) {
    // Aloca um novo endereço pra armazenar as novas informações e verifica
    //  se houve sucesso.
    Node *aux = (Node *) malloc(sizeof(Node));
    if (!aux) return 0;   // Caso falhe, retorna falha

    // Inicializa os campos com os valores pré determinados
    aux->next = current;
    aux->card.numPeople = target.numPeople;
    aux->card.temperature = target.temperature;

    // Associa o novo espaço populado com a lista
    current = aux;
    puts("Card created");
    printf("Num: %i, Temp: %g\n", current->card.numPeople, current->card.temperature);
    return 1;   // Retorna sucesso
  }
  else if(current->card.numPeople == target.numPeople) {
    puts("Card overwrited");
    (*pNode)->card = target;
    return 1;
  }
  else return insert(&(current->next), target); // Continua procurando
}

int find(Node *node, int target) {

  static int index = 0;

  if (!node) return -1;
  else if (node->card.numPeople == target){printf("ACHOU: %i\n", node->card.numPeople); return index;}
    else {
      index++;
      return find(node, target);
    }
}
