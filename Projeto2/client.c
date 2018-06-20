#include <stdio.h>
#include <stdio_ext.h>    // fpurge
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "contacts.h"

#define BUFF_SIZE 128
#define PHONE_SIZE 11
#define NAME_SIZE 54
#define PORT_SIZE 5

#define DISTRO_NAME_SIZE 32


// Structs
typedef struct {
  pthread_t *addr;
  int ns;
  struct sockaddr_in client;
} Arguments;

struct client_data {
  char name[NAME_SIZE];
  char num[PHONE_SIZE];
};

// Funções auxiliares
void choppy( char *s ) {
    s[strcspn ( s, "\n" )] = '\0';
}

void getDistro(char distro[]) {

  char ch;    // Caracter que será lido no popen
  int i = 0;                            // Iterador


  // Obtém a distro
  FILE *fp = popen("cat /etc/issue", "r");

  // Coleta apenas o nome da distro
  while( (ch = fgetc(fp)) != ' ') distro[i++] = ch;
  // Fecha o ponteiro
  pclose(fp);
}

void invokeTerminal(char distro[], char *file, char *args) {

  char command[DISTRO_NAME_SIZE];

  // Invoca o terminal correto de acordo com a distro
  if(!strcmp(distro, "Deepin"))
    strcpy(command, "deepin-terminal -x ");

  else if(!strcmp(distro, "Kubuntu"))
    strcpy(command, "konsole -x ");

  else if(!strcmp(distro, "Ubuntu"))
    strcpy(command, "gnome-terminal -x ");

  else puts("Sistema nao suportado.");

  // Concatena o programa que será executado com args
  strcat(command, file);
  strcat(command, " -e ");
  strcat(command, args);
  printf("Comando a ser executado no terminal: %s\n", command);

  system(command);
}

void *threadClient(void *args_connect) {
  puts("Entrou no thread client");
  char distro[DISTRO_NAME_SIZE];        // Strings q irão conter a distro e o comando
  char termArgs[DISTRO_NAME_SIZE];
  char outterSocket[sizeof(int)];
  Arguments *params;      // Argumentos coletados da thread


  params = (Arguments *) args_connect;
  free(args_connect);

  getDistro(distro);

  printf("Distribuição linux deste computador: %s\n", distro);

  snprintf(outterSocket, sizeof(int), "%i", params->ns);

  // Coleta dados do cliente
  strcpy(termArgs, "./chat");      // Inicializa a string com o arquivo que será executado

  invokeTerminal(distro, termArgs, outterSocket);

  return 0;
}

bool write_file(char type, char  *stringNum) {

    FILE *fp;
    fp = fopen("contatos.txt","a+");

    if( fp )
      fprintf(fp, "%c-%s\n", type, stringNum);
    else
      return false;

    printf("O que foi escrito: %c-%s\n", type, stringNum);
    fclose(fp);

    return true;
}

char *read_file() {

  FILE *fp;
  char *ans = NULL;
  char *line = NULL;
  size_t len = 0;

  // Aparentemente o endereço de ans quando inicializada
  //  como vetor fica mudando, então fiz por alocação
  //    dinâmica mesmo.
  ans = (char *) malloc(1024);

  fp = fopen("contatos.txt", "r");

  if (fp)
    while ((getline(&line, &len, fp)) != EOF)
      strcat(ans, line);

  fclose(fp);

  return ans;
}

void printMenu() {
  puts("Selecione uma das opções a seguir:");
  puts("1 - Conectar a um número;");
  puts("2 - Conectar a um grupo;");
  puts("3 - Sair");
}

void outterConnection(int mainSocket){

  int outterSocket;
  socklen_t clientLen;
  Arguments *params;
  struct sockaddr_in outterClient;
  puts("Entrou no outter conection");

  clientLen = sizeof(outterClient);
  // Aceitando conexões externas
  if ( (outterSocket = accept(mainSocket, (struct sockaddr *)&outterClient, &clientLen)) == -1) {
    perror("Accept()");
    exit(5);
  }

  puts("Esse cliente aceitou uma conexão!");

  params = (Arguments *) malloc(sizeof(Arguments));
  params->addr = (pthread_t *) malloc(sizeof(pthread_t));
  params->client = outterClient;
  params->ns = outterSocket;

  puts("Quem está conectando:");
  printf("IP: %s\nPorta: %i\n", inet_ntoa(outterClient.sin_addr), ntohs(outterClient.sin_port));

  pthread_create(params->addr, NULL, &threadClient, (void *)params);
  pthread_detach(*params->addr);
}

int requestLocal(int newSocket, char *sendbuf, struct sockaddr_in *user) {

  struct sockaddr_in client;


  // Envia ao server o número que o user quer conectar
  send(newSocket, sendbuf, BUFF_SIZE, 0);
  printf("Enviou ao servidor o requestLocal: %s\n", sendbuf);

  recv(newSocket, &client, BUFF_SIZE, 0);

  if(client.sin_port != 0) {
    printf("IP: %s, Porta: %i", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    *user = client;
  }

  if(client.sin_port != 0) return 1;
  else return 0;
}

void connectClient(int newSocket, char *sendbuf) {

  int outterSocket;
  Arguments *params = NULL;
  struct sockaddr_in user;


  if(  requestLocal(newSocket, sendbuf, &user) != 0 ) {

    // Cria um socket TCP
    if ((outterSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket()");
      exit(3);
    }

    // Conexão com cliente de fora
    printf("ConnectClient: %d\n", outterSocket);
    connect(outterSocket, (struct sockaddr *)&user, sizeof(user));

    params = (Arguments *) malloc(sizeof(Arguments));
    params->addr = (pthread_t *) malloc(sizeof(pthread_t));
    params->client = user;
    params->ns = outterSocket;

    pthread_create(params->addr, NULL, &threadClient, (void *)params);
    pthread_detach(*params->addr);
  }
  else
    puts("O usuário está offline no momento.");
}

void *connectionServer(void *args) {

  char num[PHONE_SIZE], name[NAME_SIZE], rcvbuf[BUFF_SIZE];
  char ans;
  Contact myLocal;
  Arguments *params;
  struct client_data msg;


  params = (Arguments *) args;
  free(args);

  printf("Cheguei até aqui connectServer\n");
  do {

    puts("Para papear, logue já!");
    puts("Digite seu número de tefelone:");
    __fpurge(stdin);
    fgets(num, PHONE_SIZE, stdin);

    puts("Digite seu nome:");
    __fpurge(stdin);
    fgets(name, NAME_SIZE, stdin);

    printf("Número digitado: %s.\nNome digitado: %s.\nO número está correto? (S/n) ", num, name);
    ans = getchar();
    choppy(num);

  } while(ans == 'n' || ans == 'N');

  // strcpy(myLocal.name, name);
  strcpy(myLocal.num, num);
  myLocal.local = params->client;

  // Envia o número para o cadastro
  if ( send(params->ns, &myLocal, sizeof(msg), 0) < 0 ) {
    perror("Send()");
    exit(5);
  }

  // Recebe feedback de cadastro
  if (recv(params->ns, rcvbuf, BUFF_SIZE, 0) < 0) {
    perror("Recv()");
    exit(6);
  }

  // Exibe menu
  int option;
  if(strcmp(rcvbuf, "Success") == 0) {
    while(strcmp(rcvbuf, "Shutdown") != 0) {

      printMenu();
      scanf(" %i", &option);

      switch(option){
        case 1:
          // Conecta em um número
          puts("Digite o número que quer conectar:");
          scanf(" %s", num);
          printf("Número resgatado: %s\n", num);
          connectClient(params->ns, num);

          break;
        case 3:
          strcpy(num, "Shutdown");
          send(params->ns, num, PHONE_SIZE, 0);
          recv(params->ns, &rcvbuf, PHONE_SIZE, 0);
          break;
      }
    }
  }

  return 0;
}

int main(int argc, char **argv) {

  int s, mainSocket;
  struct hostent *hostnm;
  struct sockaddr_in server, thisClient;


  // SERVIDOR DO CLIENTE----------------------------------
  // Define o endereço IP e a porta do servidor
  thisClient.sin_family      = AF_INET;
  thisClient.sin_port        = htons(INADDR_ANY);
  thisClient.sin_addr.s_addr = INADDR_ANY;

  mainSocket = socket(PF_INET, SOCK_STREAM, 0);
  bind(mainSocket, (struct sockaddr *) &thisClient, sizeof(thisClient));
  listen(mainSocket, 1);
  // FIM SERVIDOR DO CLIENTE------------------------------

  // O primeiro argumento (argv[1]) é o hostname do servidor.
  // O segundo argumento (argv[2]) é a porta do servidor.
  if(argc != 3) {
    fprintf(stderr, "Use: %s hostname porta\n", argv[0]);
    exit(1);
  }

  // Obtendo o endere�o IP do servidor
  hostnm = gethostbyname(argv[1]);
  if (hostnm == (struct hostent *) 0) {
    fprintf(stderr, "Gethostbyname failed\n");
    exit(2);
  }

  server.sin_family      = AF_INET;
  server.sin_port        = htons(atoi(argv[2]));
  server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

  // Cria um socket TCP (stream)
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket()");
    exit(3);
  }

  // Estabelece conexão com o servidor
  printf("Server: %d\n", s);
  if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
     perror("Connect()");
     exit(4);
  }

  Arguments *params;
  params = (Arguments *) malloc(sizeof(Arguments));
  params->addr = (pthread_t *) malloc(sizeof(pthread_t));
  params->client = thisClient;
  params->ns = s;

  pthread_create(params->addr, NULL, &connectionServer, (void *)params);
  pthread_detach(*params->addr);

  while(1){
    outterConnection(mainSocket);
  }

  printf("Cliente terminou com sucesso.\n");
}
