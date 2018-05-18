#ifndef LIST_H
#define LIST_H

typedef struct weather_t {
  int numPeople;
  float temperature;
} Weather;

typedef struct  node {
  Weather card;
  struct node *next;
} Node;

int insert(Node **, Weather);

/* Busca para verificar se já existe uma correspondência no BD
 * Retorna: inteiro;
 *  Index no sucesso | -1 na falha
 */
int find(Node *, int);

#endif
