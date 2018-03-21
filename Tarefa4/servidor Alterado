#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>

/* Constantes do programa */
#define NAME_SIZE 20
#define MSG_SIZE 80
#define LIST_SIZE 10
#define ANS_SIZE 1250

/*
* Struct para salvar mensagens
*/

typedef struct Msg {
	char name [NAME_SIZE];
	char message [MSG_SIZE];
	char answer [ANS_SIZE];
	int type;
}Msg;

typedef struct {
	int ns;
	struct sockaddr_in client;
	struct sockaddr_in server;
	Msg envio;
} Arguments;

/* Variaveis globais */
Msg list[10];		/* lista de mensagens		*/
int pList = 0;	/* numero de mensagens cadastradas */

// Funções auxiliares

// Procura e armazena nova msg
int insertMsg(Msg list[], Msg *envio){

	int i;

	// procurando espaco vazio e atribuindo nova msg
	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, "") == 0){
			list[i] = *envio;
			strcpy(envio->answer, "\nMensagem incluida com sucesso!");
			envio->type = 0;
			return 1;
		}
	}

	strcpy(envio->answer, "A mensagem nao foi incluida. A lista ja esta cheia!");
	return 0;
}

int removeMsg(Msg list[], int pList, Msg *envio){

	int i, rmvdMsgs = 0;
	char res[ANS_SIZE] = "";


	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, envio->name) == 0){

			// salvando valores
			strcat(res, "Usuario: ");
			strcat(res, list[i].name);
			strcat(res, " | ");
			strcat(res, "Mensagem: ");
			strcat(res, list[i].message);

			//removendo
			strcpy(list[i].name, "\0");
			rmvdMsgs++;
		}
	}

	strcpy(envio->answer, res);
	if(rmvdMsgs == 0)
		strcat(envio->answer, "Nao existem mensagens desse usuario.");
	envio->type = rmvdMsgs;
	return rmvdMsgs;
}

int getMsgs(Msg list[], int pList, Msg *envio) {

	char res[ANS_SIZE] = "";
	int i, count = 0;

	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, "\0") != 0){
			strcat(res, "Usuario: ");
			strcat(res, list[i].name);
			strcat(res, " | ");
			strcat(res, "Mensagem: ");
			strcat(res, list[i].message);
			count++;
		}
	}

	envio->type = count;
	strcpy(envio->answer, res);
}

void *response(void *args){

	Arguments *aux = NULL;
	int newSocket;
	int fin = 0;
	struct sockaddr_in *client;
	struct sockaddr_in *server;
	Msg *envio;


	// Atribuicoes
	aux = (Arguments *) args; 	/* Resgatando informacoes dos parametros */

	newSocket 	= aux->ns;

	while(1) {
    /* Recebe uma mensagem do cliente atraves do novo socket conectado */
    if (recv(newSocket, &aux->envio, sizeof(aux->envio), 0) == -1) {
        perror("Recv()");
        exit(6);
    }

    puts("Mensagem recebida do cliente:");
		printf("Tipo: %i | Nome: %s | Msg: %s\n", aux->envio.type, aux->envio.name, aux->envio.message);

		switch(aux->envio.type) {
			case 1:
				if(insertMsg(list, &aux->envio) == 1);
					pList++;
				break;
			case 2:
				getMsgs(list, pList, &aux->envio);
				break;
			case 3:
				pList -= removeMsg(list, pList, &aux->envio);
				break;
			case 4:
				puts("Cliente finalizado com sucesso.");
				aux->envio.type = -1;
				fin = 1;
				break;
		}

    /* Envia uma mensagem ao cliente atraves do socket conectado */
    if (send(newSocket, &aux->envio, sizeof(aux->envio), 0) < 0) {
        perror("Send()");
        exit(7);
    }

    printf("Mensagem enviada ao cliente: %s\n", aux->envio.answer);

		if(fin == 1){
			puts("Thread finalizado");
			shutdown(newSocket, SHUT_RDWR);
			close(newSocket);
			pthread_exit(0);
		}

    /*Limpando pra proxima*/
    strcpy(aux->envio.name, " ");
    strcpy(aux->envio.message, " ");
    strcpy(aux->envio.answer, " ");
    aux->envio.type = 0;
	}
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
    struct sockaddr_in client;
    struct sockaddr_in server;
    int s;					/* Socket para aceitar conexoes */
    int ns;					/* Socket conectado ao cliente */
    int namelen;		/* tamanho do nome */
		int threads = 0;
    Msg envio;			/* struct que sera enviada */
		Arguments *params;
		pthread_t tid[10];


		// Inicializacao do vetor de mensagens
		int i;
		for(i = 0; i < LIST_SIZE; i++){
			strcpy(list[i].name, "");
			strcpy(list[i].message, "");
		}

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
			params = (Arguments *)malloc(sizeof(Arguments));
			params->ns 		= ns;
			params->client = client;
			params->server = server;
			params->envio 	= envio;

			// Criacao da thread
			pthread_create(&(tid[threads++]), NULL, &response, (void *)params);
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
