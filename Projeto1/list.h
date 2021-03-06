#ifndef LIST_H
#define LIST_H
#include <stdbool.h>


struct weather_t {
  int numPeople;
  float temperature;
  bool flag;
};

struct node_t {
  struct weather_t card;
  struct node_t *next;
};

typedef struct weather_t Weather;
typedef struct node_t Node;

int insert(Node **, Weather);

/* Busca para verificar se já existe uma correspondência no BD
 * Retorna: temperatura imediatamente anterior ao target (float)
 */
float findAdjacents(Node *, float);

void printList(Node *);
#endif
