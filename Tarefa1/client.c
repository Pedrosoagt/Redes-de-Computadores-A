#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 2000

void main(argc, argv)
int argc;
char **argv;
{
    // Descritor do socket
    int s;

    // Indicador da porta
    int port;

    // Tamanho da mensagem recebida do usuário
    int lenMessage;

    //  Aglomerado de informacoes sobre o servidor ao qual se comunicara
    struct sockaddr_in server;

    // tamanho do
    int length;
    char buf[BUFF_SIZE];
    int recvp = 0;
    char message[BUFF_SIZE];


    port = atoi(argv[2]);

    printf("Dados resgatados:\nIP: %s  |  Porta: %i\n", argv[1], port);

    // 1o passo: criacao do socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s < 0){
        perror("socket()");
        exit(1);
    }

    // 2o passo: obtencao das informacoes
    server.sin_family           = AF_INET;
    server.sin_port             = htons(port);
    server.sin_addr.s_addr      = inet_addr(argv[1]);


    while(strcmp(buf, "exit") != 0){
      // 3o passo: tratamento e envio do buffer
      printf("> ");
      fgets(message, BUFF_SIZE, stdin);

      // Realizando o "trim" do '\0'
      lenMessage = strlen(message);
      message[lenMessage-1] = 0;

      fflush(stdout);
      strcpy(buf, message);

      if(sendto(s, buf, (strlen(buf) + 1), 0, (struct sockaddr *)&server, sizeof server) < 0){
          perror("sendto()");
          exit(1);
      }

      if(strcmp(buf, "exit") == 0){
        printf("Ate logo!\n");
        close(s);
        exit(1);
      }

      // 4o passo: recebimento dos dados retornados pelo servidor
      length = sizeof(server);
      recvp = recvfrom(s, buf, BUFF_SIZE, 0, (struct sockaddr *)&server, &length);
      if(recvp < 0){
          perror("recvfrom()");
          exit(1);
      }

      fflush(stdout);
      printf("%s\n", buf);

    }

    close(s);
}
