#ifndef LIST_H
#define LIST_H

typedef struct status {
  int numPeople;
  float temperature;
} Status;

typedef struct  node {
  Status state;
  int *next;
} Node;

int insert(Node *, Status);

/* Busca para verificar se já existe uma correspondência no BD
 * Retorna: inteiro;
 *  Index no sucesso | -1 na falha
 */
int find(Node *, int);

#endif
