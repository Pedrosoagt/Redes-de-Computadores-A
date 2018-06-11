#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <pthread.h>
#include <string.h>

#define BUFF_SIZE 128

// Structs
typedef struct {
	pthread_t *addr;
	int ns;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int index;
} Arguments;

typedef struct node_t {
	struct node_t *next;
} Node;

void *threadClient(void *args) {
	puts("Thread do cliente aberta para chat");
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

char * read_file() {

  FILE *fp;
  char *ans = NULL;
  char *line = NULL;
  size_t len = 0;

  // Aparentemente o endereço de ans quando inicializada
  //  como vetor fica mudando, então fiz por alocação
  // dinâmica mesmo.
  ans = (char *) malloc(1024);

  fp = fopen("contatos.txt", "r");

  if (fp)
    while ((getline(&line, &len, fp)) != EOF)
      strcat(ans, line);

  fclose(fp);

  return ans;
}

void printMenu(){
  puts("Selecione uma das opções a seguir:");
  puts("1 - Conectar a um número;");
  puts("2 - Conectar a um grupo;");
  puts("3 - Sair");
}

bool requestLocal(int newSocket, char *sendbuf){

  char aux[BUFF_SIZE];

  // Envia ao server o número que o user quer conectar
  send(newSocket, sendbuf, BUFF_SIZE, 0);
	printf("Enviou ao servidor o requestLocal: %s\n", sendbuf);

  recv(newSocket, &aux, BUFF_SIZE, 0);

	printf("Recebeu do servidor no requestLocal: %s\n", aux);

	if(strcmp(aux, "User offline") != 0) return true;
	else return false;
}

void connectClient(int newSocket, char *sendbuf){

  Arguments *params = NULL;

  struct sockaddr_in user;
	if(	requestLocal(newSocket, sendbuf) ){

	  params = (Arguments *) malloc(sizeof(Arguments));
	  params->addr = (pthread_t *) malloc(sizeof(pthread_t));
	  params->client = user;
	  params->ns = 0;
	  params->index = 0;

	  pthread_create(params->addr, NULL, &threadClient, (void *)params);
	  pthread_detach(*params->addr);
	}
	else
		puts("O usuário está offline no momento.");
}

bool sendMessage(){}

int main(argc, argv)
int argc;
char **argv;
{
  unsigned short port;
  struct hostent *hostnm;
  struct sockaddr_in server;
  int s;
  float status = -1;
  char ans;
  char sendbuf[BUFF_SIZE];
  char rcvbuf[BUFF_SIZE];

  /*
	 * O primeiro argumento (argv[1]) � o hostname do servidor.
	 * O segundo argumento (argv[2]) � a porta do servidor.
	 */
  if(argc != 3){
    fprintf(stderr, "Use: %s hostname porta\n", argv[0]);
    exit(1);
  }

  /*
	 * Obtendo o endere�o IP do servidor
	 */

   hostnm = gethostbyname(argv[1]);
   if (hostnm == (struct hostent *) 0) {
     fprintf(stderr, "Gethostbyname failed\n");
     exit(2);
   }
   port = (unsigned short) atoi(argv[2]);

   /*
 	 * Define o endere�o IP e a porta do servidor
 	 */
   server.sin_family      = AF_INET;
   server.sin_port        = htons(port);
   server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

   /*
 	 * Cria um socket TCP (stream)
 	 */
 	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
 		perror("Socket()");
 		exit(3);
 	}

 	/* Estabelece conex�o com o servidor */
  if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
 	  perror("Connect()");
 		exit(4);
 	}

  do {

    puts("Para papear, logue já!");
    printf("Digite seu número de tefelone:\n");
    __fpurge(stdin);
    fgets(sendbuf, BUFF_SIZE, stdin);

    printf("O número digitado foi: %s.\nVc tem certeza desse número? (S/n) ", sendbuf);
    ans = getchar();

  } while(ans == 'n' || ans == 'N');

	// Envia o número para o cadastro
  if (send(s, sendbuf, BUFF_SIZE, 0) < 0) {
    perror("Send()");
    exit(5);
  }

	// Recebe feedback de cadastro
  if (recv(s, rcvbuf, BUFF_SIZE, 0) < 0) {
    perror("Recv()");
    exit(6);
  }

	// Exibe menu
	int option;
	if(strcmp(rcvbuf, "Success") == 0){
		while(strcmp(rcvbuf, "Shutdown") != 0) {

			printMenu();
	    scanf(" %i", &option);

	    switch(option){
	      case 1:
	        // Conecta em um número
	        puts("Digite o número que quer conectar:");
	        scanf(" %s", sendbuf);
					connectClient(s, sendbuf);

					break;
				case 3:
					strcpy(sendbuf, "Shutdown");
					send(s, sendbuf, BUFF_SIZE, 0);
					recv(s, &rcvbuf, BUFF_SIZE, 0);
					break;
	    }
		}
	}

	printf("Cliente terminou com sucesso.\n");
}
