//Server

//adicionar na lista online
//verificar numero online
//retornar ip do numero requisitado se estiver online

//receber iformações - ip (socket) - num - nome

//possibilidade verificar na lista


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>		// inet_ntoa
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

struct client_data{
  char *name;
  char *num;
};


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
void sendSckAddr(int ns, struct sockaddr_in *data) {
	ssize_t len = sizeof(struct sockaddr_in);
  send(ns, data, len, 0);
}

void sendString(int ns, const char *data) {
  send(ns, data, BUFF_SIZE, 0);
}

void rcvNum(int ns, char *buf){
  recv(ns, buf, BUFF_SIZE, 0);
}

// Cadastra um novo usuário; True para sucesso, False para falha
int signup(Arguments *aux_params, char rcvbuf[]){

  // Na primeira vez, todos os clientes devem se identificar para o cadastro inicial
  Contact newContact;
  newContact.local = aux_params->client;
  strcpy(newContact.num, rcvbuf);

  lockMutex();    // Prevencao de dados inconsistentes

  // Insere o novo contato na lista de contatos
  if(createContact(&contactList, newContact)) {
    unlockMutex();  // Prevencao de dados inconsistentes
    return true;
  }
  else {
    unlockMutex();  // Prevencao de dados inconsistentes
    return false;
  }
}

void *response(void *args){

  Arguments *aux_params = NULL;
  int newSocket;
  char sendbuf[BUFF_SIZE];
  char num[BUFF_SIZE];
  struct client_data msg;


  // Atribuicoes
  aux_params = (Arguments *) args;   /* Resgatando informacoes dos parametros */

  newSocket = aux_params->ns;
  printf("Conexão feita IP: %s | Porta: %i\n", inet_ntoa(aux_params->client.sin_addr), ntohs(aux_params->server.sin_port));

  /* Recebe uma mensagem do cliente atraves do novo socket conectado */
  if (recv(newSocket, &msg, BUFF_SIZE, 0) == -1) {
      perror("Recv()");
      exit(6);
  }

  // Se o usuário já está cadastrado, reconhece
  if( findContact(contactList, msg.num) )
    strcpy(sendbuf, "Welcome back!");
  else  // Se não, cadastra
    if( signup(aux_params, msg.num) ) strcpy(sendbuf, "Success");
    else strcpy(sendbuf, "Failed");

  // Envia uma mensagem ao cliente atraves do socket conectado
  sendString(newSocket, sendbuf);
  printf("Dado enviado: %s\n", sendbuf);

	// Printa os usuários cadastrados
  puts("----------------- Users Online -------------------");
  printContacts(contactList);
  puts("--------------------------------------------------");

	// Fica na espera das próximas requisições do cliente
  while( strcmp(num, "Shutdown") != 0 ){

    rcvNum(newSocket, num);
    printf("Dado recebido: %s\n", num);

    ContactCollection *contact = findContact(contactList, num);
    contact ? sendSckAddr(newSocket, &contact->info.local) : sendString(newSocket, "User offline");
  }

  shutdown(newSocket, SHUT_RDWR);
  pthread_exit(0);
}

/*
 * Servidor TCP
 */
int main(int argc, char **argv) {

  unsigned short port;
  int s;                      /* Socket para aceitar conexoes */
  int ns;                     /* Socket conectado ao cliente */
  int indexMain = 0;
  Arguments *params;
	socklen_t namelen;                /* tamanho do nome */
  struct sockaddr_in client;
  struct sockaddr_in server;


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
    if ( (ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1) {
        perror("Accept()");
        exit(5);
    }

    // Popula os valores da variavel de argumentos
    params           = (Arguments *)malloc(sizeof(Arguments));
    params->index    = indexMain++;
    params->ns       = ns;
    params->client   = client;
    params->server   = server;
    params->addr     = (pthread_t *) malloc(sizeof(pthread_t));

    // Criacao da thread
    pthread_create(params->addr, NULL, &response, (void *)params);
    pthread_detach(*params->addr);
  }

  // Finaliza Mutex
  finalizMutex();
}
