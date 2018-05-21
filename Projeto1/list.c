/* Implementação da biblioteca list.h */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"


int insert(Node **pNode, Weather target) {

  puts("INSERT: Begin insert");

  // Se for uma posição vazia, já houver um número existente
  if(!(*pNode)) {
    // Aloca um novo endereço pra armazenar as novas informações e verifica
    //  se houve sucesso.
    Node *aux = (Node *) malloc(sizeof(Node));
    if (!aux) return 0;   // Caso falhe, retorna falha
    printf("INSERT: Endereço de Aux: %li\n", aux);

    // Inicializa os campos com os valores pré determinados
    aux->next = NULL;
    aux->card = target;

    // Associa o novo espaço populado com a lista
    (*pNode) = aux;
    puts("INSERT: Card created");
    printf("INSERT: Num: %i, Temp: %f\n", (*pNode)->card.numPeople, (*pNode)->card.temperature);
    return 1;   // Retorna sucesso
  }
  else if((*pNode)->card.numPeople == target.numPeople) {
      puts("INSERT: Card overwrited");
      (*pNode)->card = target;
      return 1;
  }
  else{
    printf("INSERT: Procurando valor\n");
    return insert(&((*pNode)->next), target); // Continua procurando
  }
}

int find(Node *node, int target) {
  puts("FIND: Begin Find");
  static int index = 0;

  if (!node){
     return -1;
   }
  else if (node->card.numPeople == target){printf("FIND: ACHOU\n"); return index;}
  else {
    index++;
    return find(node->next, target);
  }
}
