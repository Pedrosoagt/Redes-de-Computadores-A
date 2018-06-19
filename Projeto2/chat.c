// ./chat <SocketPointer>
// Features: Mudar nome do terminal para nome do usuário via code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdbool.h>

#define MSG_SIZE 280
#define SELECTOR '!'


void *receiver(void *args) {

  int *outterSocket = (int *) args;
  size_t len;
  char msg[MSG_SIZE];

  len = sizeof(msg);
  while(1) {
    recv(*outterSocket, msg, len, 0);
    printf("%s", msg);
  }
}

void printOptions(){

  puts("Envie mensagens a vontade!");
  puts("Para enviar fotos, digite !photo <endereçoDaImagem>");
  puts("Para sair, digite !quit");
}

int getCommand (char *msg) {

  char command[5];
  int i = 1;


  while(msg[i] != ' ') command[i - 1] = msg[i];
  if( strcmp(command, "photo") == 0) return 1;
  else if (strcmp(command, "quit") == 0) return 2;
  else return 0;
}

char *getMessage() {

  char *msg;


  msg = (char *) malloc(MSG_SIZE);
  scanf("%s", msg);

  // Tirar espaços antes da mensagem
  while (*msg == ' ') msg += 1;

  return msg;
}

void sendString(int outterSocket, char *msg){
  send(outterSocket, msg, MSG_SIZE, 0);
}

void sendPhoto(int outterSocket) {

}

void menu(int outterSocket){

  int commandResult;
  char msg[MSG_SIZE];
  bool quit = false;

  printOptions();
  do {

    strcpy(msg, getMessage());

    switch (msg[0]) {

      case SELECTOR:
        commandResult = getCommand(msg);
        if(commandResult == 1)
          sendPhoto(outterSocket);
        else if(commandResult == 2)
          quit = true;
        else
          puts("Comando inválido!");
        break;
      default:
        sendString(outterSocket, getMessage());
        break;
    }
  } while(quit != true);
}


int main(int argc, char **args){

  int outterSocket = atoi(args[2]);

  menu(outterSocket);
}
