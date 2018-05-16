#include <stdio.h>
#include <math.h>
#include "list.h"


/* Escolhe a temperatura de acordo com o BD
 * Retorna: Temperatura escolhida (float)
 */
float chooseTemp(Status states[], int numPeople) {

  int index;
  int statesLen = sizeof(states) / sizeof(states[0]);

  index = find(states, 0, statesLen, numPeople);

  if (index != -1)
    return states[index].temperature;

  else {
    float prevTemp, nextTemp;
    float result = sqrt((pow(prevTemp, 2) + pow(nextTemp, 2)) / 2);
    return result;
  }
}

void main() {

  // Inicialização do vetor que será usado como BD
  Node *states = NULL;

  Status ex = NULL;
  ex.numPeople = 14;
  ex.temperature = 25.7;

  insert(states, ex);
  print("Numero de pessoas: %i, Temp: %f", states->state.numPeople, states->state.temperature);
}
