#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>
#include <math.h>
#include "list.h"


/* Constants */
#define MAX_RANGE 3


/*Variaveis globais*/
struct sockaddr_in *ipHardware, *ipUser;
int counter = 0;
Node *list = NULL;  /* Array de temperaturas */
float roomTemp = 25;

/* Algoritmo:
	Ao pedir temp:
		Olha termometro;
		Se for condizente, seta
		se não, sugere uma temp melhor
*/

typedef struct {
	pthread_t *addr;
	int ns;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int index;
} Arguments;

// Aux functions

// Envia dados pela função send
void sendData(int ns, float *data) {
	send(ns, &data, sizeof(*data), 0);
}

// Verifica se uma temperatura está próxima da outra
int isCloseTo(float reqTemp, float roomTemp){
	return abs(reqTemp - roomTemp) <= MAX_RANGE ? 1 : 0;
}

//verifica se houve mudança na base de dados
int requestClient(float *reqTemp, float roomTemp) {

	if(isCloseTo(*reqTemp, roomTemp)) {

		Weather aux;
		aux.numPeople = counter;
		aux.temperature = *reqTemp;
		if( insert(&list, aux) ) puts("Nova temperatura inserida");
		else puts("Falha na insercao da nova temperatura");
		return 1;
	}
	else {
		puts("Recebeu como arduíno");
		/*
		// Sugere outra temp melhor
		*reqTemp =
		return 0;
		*/
	}
}

void *response(void *args) {

	Arguments *aux = NULL;				// Argumentos das Threads
	struct sockaddr_in *client;
	struct sockaddr_in *server;
	int newSocket;
	int status;
	int validTemp;
	float tp;
	char sendbuf[8];


	// Atribuicoes
	aux = (Arguments *) args; 	/* Resgatando informacoes dos parametros */

	newSocket = aux->ns;
	printf("Conexão feita IP: %i Porta: %i\n", aux->client.sin_addr, ntohs(aux->server.sin_port));

	while(1) {
		/* Recebe uma mensagem do cliente atraves do novo socket conectado */
		if ((recv(newSocket, &tp, sizeof(float), 0) && recv(newSocket, &status, sizeof(int), 0)) == -1) {
	    perror("Recv()");
	    exit(6);
		}

		// Identifica a origem do dado
		status == 1 ? ++counter : (status == 0 ? --counter : (validTemp = requestClient(&tp, roomTemp)));

		/*-----------------*/
		Weather ex;

		ex.numPeople = 15;
		ex.temperature = 23.7;

		insert(&list, ex);

		// Status -1, controla para send de usuário
		if(status =! -1) {
			if(validTemp)
			 	sendData(newSocket, &tp);
		}else {
			sendData(newSocket, &tp);
		}

		if( find(list, 22) ) puts("Achouuuu");
		else puts("Eroooooooow");

		printf("Num: %i, Temp: %g", list->card.numPeople, list->card.temperature);
	}
}

/*
 * Servidor TCP
 */
void main(argc, argv)
int argc;
char **argv; {
  unsigned short port;
  char sendbuf[12];
  char recvbuf[12];
  struct sockaddr_in client;
  struct sockaddr_in server;
  int s;					/* Socket para aceitar conexoes */
  int ns;					/* Socket conectado ao cliente */
  int namelen;			/* tamanho do nome */
	int threads = 0;
	Arguments *params;
	pthread_t tid;
	int indexMain = 0;

  /*
   * O primeiro argumento (argv[1]) eh a porta
   * onde o servidor aguardara por conexoes
  */
  if (argc != 2) {
      fprintf(stderr, "Use: %s porta\n", argv[0]);
      exit(1);
  }

  port = (unsigned short) atoi(argv[1]);

  /*
   * Cria um socket TCP (stream) para aguardar conexoes
   */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket()");
      exit(2);
  }

 /*
  * Define a qual endere�o IP e porta o servidor estara ligado.
  * IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
  * os enderecos IP
  */
  server.sin_family = AF_INET;
  server.sin_port   = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;

  /*
   * Liga o servidor à porta definida anteriormente.
   */
  if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
     perror("Bind()");
     exit(3);
  }

  /*
   * Prepara o socket para aguardar por conexoes e
   * cria uma fila de conexoes pendentes.
   */
  if (listen(s, 1) != 0) {
      perror("Listen()");
      exit(4);
  }

  /*
   * Aceita uma conexao e cria um novo socket atraves do qual
   * ocorrera a comunicacao com o cliente.
   */
  namelen = sizeof(client);
	while(1){
    if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1) {
        perror("Accept()");
        exit(5);
    }

		// Popula os valores da variavel de argumentos
		params 					= (Arguments *)malloc(sizeof(Arguments));
		params->index 	= indexMain++;
		params->ns 			= ns;
		params->client 	= client;
		params->server 	= server;
		params->addr 		= (pthread_t *) malloc(sizeof(pthread_t));

		// Criacao da thread
		pthread_create(params->addr, NULL, &response, (void *)params);
		pthread_detach(*params->addr);
		// if (err != 0)
		// 		printf("\ncan't create thread :[%s]", strerror(err));
		// else
		// 		printf("\n Thread created successfully\n");
	}

  /* Fecha o socket aguardando por conexoes */
  close(s);

  printf("Servidor terminou com sucesso.\n");
  exit(0);
}