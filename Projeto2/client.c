#include <stdio.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>

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
  char *name;
  char *num;
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

void invokeTerminal(char *distro, char *termArgs) {

  char command[DISTRO_NAME_SIZE];

  // Invoca o terminal correto de acordo com a distro
  if(!strcmp(distro, "Deepin"))
    strcpy(command, "deepin-terminal -e ");

  else if(!strcmp(distro, "Kubuntu"))
    strcpy(command, "konsole -e ");

  else if(!strcmp(distro, "Ubuntu"))
    strcpy(command, "gnome-terminal -e ");

  else puts("Sistema nao suportado.");

  // Concatena o programa que será executado com args
  strcat(command, termArgs);

  system(command);
}

void *threadClient(void *args_connect) {

  char distro[DISTRO_NAME_SIZE];        // Strings q irão conter a distro e o comando
  char termArgs[DISTRO_NAME_SIZE];
  char port[PORT_SIZE];
  struct sockaddr_in *client_addr;      // Argumentos coletados da thread

  client_addr = (struct sockaddr_in *) args_connect;

  getDistro(distro);

  printf("Distro: %s\n", distro);

  snprintf(port, PORT_SIZE, "%d", client_addr->sin_port);

  // Coleta dados do cliente
  strcpy(distro, "");              // Inicializa como string vazia
  strcpy(termArgs, "./chat ");      // Inicializa a string com o arquivo que será executado
  strcat(termArgs, inet_ntoa(client_addr->sin_addr));
  strcat(termArgs, " ");
  strcat(termArgs, port);

  printf("termArgs: %s\n", termArgs);

  invokeTerminal(distro, termArgs);

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

bool requestLocal(int newSocket, char *sendbuf) {

  char aux[BUFF_SIZE];

  // Envia ao server o número que o user quer conectar
  send(newSocket, sendbuf, BUFF_SIZE, 0);
  printf("Enviou ao servidor o requestLocal: %s\n", sendbuf);

  recv(newSocket, &aux, BUFF_SIZE, 0);

  printf("Recebeu do servidor no requestLocal: %s\n", aux);

  if(strcmp(aux, "User offline") != 0) return true;
  else return false;
}

void connectClient(int newSocket, char *sendbuf) {

  Arguments *params = NULL;

  struct sockaddr_in user;
  if(  requestLocal(newSocket, sendbuf) ) {

    params = (Arguments *) malloc(sizeof(Arguments));
    params->addr = (pthread_t *) malloc(sizeof(pthread_t));
    params->client = user;
    params->ns = 0;

    pthread_create(params->addr, NULL, &threadClient, (void *)params);
    pthread_detach(*params->addr);
  }
  else
    puts("O usuário está offline no momento.");
}

void *connectionServer(void *args) {

  char num[PHONE_SIZE], name[NAME_SIZE], rcvbuf[BUFF_SIZE];
  char ans;
  Arguments *params;
  struct client_data msg;

  params = (Arguments *) args;

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

  strcpy(msg.name, name);
  strcpy(msg.num, num);

  // Envia o número para o cadastro
  if ( send(params->ns, &msg, sizeof(msg), 0) < 0 ) {
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

  struct hostent *hostnm;
  struct sockaddr_in server;
  int s;


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

  // Define o endereço IP e a porta do servidor
  server.sin_family      = AF_INET;
  server.sin_port        = htons(INADDR_ANY);
  server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

  // Cria um socket TCP (stream)
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket()");
    exit(3);
  }

  // Relaciona a porta desejada com o socket
  if (bind(s, (struct sockaddr *) &server, sizeof(server) ) < 0) {
    perror("Bind()");
    exit(3);
  }

  // Começa a esperar alguma conexão na porta atrelada
  if (listen(s, 1) != 0) {
      perror("Listen()");
      exit(4);
  }

  // Estabelece conexão com o servidor
  if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
     perror("Connect()");
     exit(4);
  }

  Arguments *params;
  params = (Arguments *) malloc(sizeof(Arguments));
  params->addr = (pthread_t *) malloc(sizeof(pthread_t));
  params->client = server;
  params->ns = s;

  pthread_create(params->addr, NULL, &connectionServer, (void *)params);
  pthread_detach(*params->addr);

  while(1){

  }

  printf("Cliente terminou com sucesso.\n");
}
