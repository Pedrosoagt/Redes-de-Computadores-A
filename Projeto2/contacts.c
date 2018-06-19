/* Implementação da biblioteca "contacts.h" */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "contacts.h"


//função para criar lista no inicio
ContactCollection * createCollection() {
  return (ContactCollection *) malloc(sizeof(ContactCollection));
}

//função para colocar um contato na lista quando o numero não existe na lista
bool createContact(ContactCollection **pNode, Contact target){

  // Se for uma posição vazia, já houver um número existente
  if(!(*pNode)) {
    // Aloca um novo endereço pra armazenar as novas informações e verifica
    //  se houve sucesso.
    ContactCollection *aux = createCollection();
    if (!aux) return false;   // Caso falhe, retorna falha

    // Inicializa os campos com os valores pré determinados
    aux->next = NULL;
    aux->info = target;

    // Associa o novo espaço populado com a lista
    (*pNode) = aux;
    puts("Contact created.");
    return true;   // Retorna sucesso
  }
  else{
    printf("Searching insertion place...\n");
    return createContact(&((*pNode)->next), target); // Continua procurando
  }
}

//função de busca na lista
ContactCollection *findContact(ContactCollection *node, char target[]) {

  puts("Searching contact...");

  if(!node) return (ContactCollection *) 0;
  if(strcmp(node->info.num, target) == 0) return node;
  return findContact(node->next, target);
}

bool updateContact(ContactCollection **pNode, Contact target) {

  ContactCollection *aux;

  aux = findContact((*pNode), target.num);
  if(!aux) return false;

  (*pNode)->info = target;
  return true;
}

void printContacts(ContactCollection *list) {
  if(list) {
    printf("Número de tel: %s\n", list->info.num);
    printContacts(list->next);
  }
}

bool removeContact(ContactCollection **pNode, Contact target) {

  // Se alcançou o final, retorna falha
  if(!(*pNode)) return false;
  // Caso contrario, verifica se o item atual é igual ao procurado
  else if(strcmp((*pNode)->info.num, target.num) == 0) {
    ContactCollection *aux;
    aux = (*pNode);
    (*pNode) = (*pNode)->next;
    free(aux);
    return true;
  }
  // Se não for, continue procurando
  return removeContact(&((*pNode)->next), target);
}
