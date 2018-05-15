#include <stdio.h>

// Define struct a ser utilizada no BD
typedef struct {
  int numPeople;
  float temperature;
} Status;

/* Busca binária para verificar se já existe uma correspondência no BD
 * Retorna: inteiro;
 *  Index no sucesso | -1 na falha
 */
int find(Status states[], int begin, int end, int target) {

  // Caso o fim ainda não tenha sido alcançado
  if (end >= 1) {

    // Encontra o meio atual
    int mid = begin + (end - begin) / 2;

    // Se o alvo foi encontrado, retorna seu index
    if(states[mid].numPeople == target) return mid;

    // Se não for encontrado e o alvo for maior que o item atual, procura na frente
    if(states[mid].numPeople > target)
      return find(states, mid + 1, end, target);

    // Se o alvo for menor que o item atual, procura atrás
    return find(states, 0, mid - 1, target);
  }

  // Caso nada seja encontrado, retorna -1
  return -1;
}

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
  Status *states = NULL;


}
