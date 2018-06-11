#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>
#include "contacts.h"

// Constantes
#define BUFF_SIZE 128

// Structs
typedef struct {
	pthread_t *addr;
	int ns;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int index;
} Arguments;

/*Variaveis globais*/
ContactCollection *contactList = NULL;


// Aux functions

pthread_mutex_t mutex;

void inicializaMutex() {
	pthread_mutex_init(&mutex, NULL);
}

void finalizMutex(){
	pthread_mutex_destroy(&mutex);
}

void lockMutex() {
	pthread_mutex_lock(&mutex);
}

void unlockMutex() {
	pthread_mutex_unlock(&mutex);
}

// Envia dados pela função send
void sendData(int ns, char *data) {
	send(ns, data, BUFF_SIZE, 0);
}

// Cadastra um novo usuário; True para sucesso, False para falha
int signup(Arguments *aux, char rcvbuf[]){

	// Na primeira vez, todos os clientes devem se identificar para o cadastro inicial
	Contact newContact;
	newContact.local = aux->client;
	strcpy(newContact.num, rcvbuf);

	lockMutex();		// Prevencao de dados inconsistentes

	// Insere o novo contato na lista de contatos
	if(createContact(&contactList, newContact)) {
		unlockMutex();	// Prevencao de dados inconsistentes
		return true;
	}
	else {
		unlockMutex();	// Prevencao de dados inconsistentes
		return false;
	}
}

void *response(void *args){

	Arguments *aux = NULL;
	int newSocket;
	bool fin = false;
  char sendbuf[BUFF_SIZE];
  char rcvbuf[BUFF_SIZE];

	// Atribuicoes
	aux = (Arguments *) args; 	/* Resgatando informacoes dos parametros */

	newSocket = aux->ns;
	printf("Conexão feita IP: %i Porta: %i\n", aux->client.sin_addr, ntohs(aux->server.sin_port));

	/* Recebe uma mensagem do cliente atraves do novo socket conectado */
	if (recv(newSocket, &rcvbuf, BUFF_SIZE, 0) == -1) {
	    perror("Recv()");
	    exit(6);
	}

	// Se o usuário já está cadastrado, reconhece
	if( findContact(contactList, rcvbuf) )
		strcpy(sendbuf, "Welcome back!");
	else	// Se não, cadastra
		if( signup(aux, rcvbuf) ) strcpy(sendbuf, "Success");
		else strcpy(sendbuf, "Failed");

	// Envia uma mensagem ao cliente atraves do socket conectado
	sendData(newSocket, sendbuf);
	printf("Dado enviado: %s\n", sendbuf);


	puts("----------------- PRINT LISTA -------------------");
	printContacts(contactList);
	puts("-------------- FINAL PRINT LISTA -----------------");

	while(strcmp(rcvbuf, "Shutdown") != 0){
		recv(newSocket, &rcvbuf, BUFF_SIZE, 0);
		printf("Dado recebido: %s\n", rcvbuf);

		puts("PASSOU NO ELSE DO SHUTDOWN");
		ContactCollection *aux = NULL;
		printf("rcvbuf do else do shutdown: %s\n", rcvbuf);
		aux = findContact(contactList, rcvbuf);
		printf("Valor de aux: %i\n", aux);
		aux ? strcpy(sendbuf, aux->info.num) : strcpy(sendbuf, "User offline");

		sendData(newSocket, sendbuf);
	}

	shutdown(newSocket, SHUT_RDWR);
	pthread_exit(0);
}


/*
 * Servidor TCP
 */
void main(argc, argv)
int argc;
char **argv;
{
  unsigned short port;
  char sendbuf[12];
  char recvbuf[12];
  int s;						/* Socket para aceitar conexoes */
  int ns;						/* Socket conectado ao cliente */
  int namelen;			/* tamanho do nome */
	int threads = 0;
	int indexMain = 0;
	pthread_t tid;
	struct sockaddr_in client;
	struct sockaddr_in server;
	Arguments *params;

	// Inicia mutex
	inicializaMutex();

 	// argv[1] = Porta em que ficará aguardando conexões
  if (argc != 2) {
      fprintf(stderr, "Use: %s porta\n", argv[0]);
      exit(1);
  }

  port = (unsigned short) atoi(argv[1]);

  // Cria um socket TCP (stream) para aguardar conexoes
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket()");
      exit(2);
  }


  // Atribui a porta desejada
  server.sin_family = AF_INET;
  server.sin_port   = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;

  // Relaciona a porta desejada com o socket
  if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
     perror("Bind()");
     exit(3);
  }

  // Começa a esperar alguma conexão na porta atrelada
  if (listen(s, 1) != 0) {
      perror("Listen()");
      exit(4);
  }

  // Aceita a conexão que for requisitada
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
	}

  /* Fecha o socket aguardando por conexoes */
  close(s);

	// Finaliza Mutex
	finalizMutex();

  printf("Servidor terminou com sucesso.\n");
  exit(0);
}
