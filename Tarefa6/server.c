#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>

/*Variaveis globais*/
int threads = 0;
int temperaturas [2]

typedef struct {
	int ns;
	struct sockaddr_in client;
	struct sockaddr_in server;
} Arguments;

void *response(void *args){

	Arguments *aux = NULL;
	int newSocket;
	int fin = 0;
	struct sockaddr_in *client;
	struct sockaddr_in *server;
	int tp;



	// Atribuicoes
	aux = (Arguments *) args; 	/* Resgatando informacoes dos parametros */

	newSocket 	= aux->ns;

	while(1) {
    /* Recebe uma mensagem do cliente atraves do novo socket conectado */
    if (recv(newSocket, &tp, sizeof(int), 0) == -1) {
        perror("Recv()");
        exit(6);
    }


    /* Envia uma mensagem ao cliente atraves do socket conectado */
    if (send(newSocket, &aux->envio, sizeof(aux->envio), 0) < 0) {
        perror("Send()");
        exit(7);
    }

    printf("Mensagem enviada ao cliente!\n");

		if(fin == 1){
			puts("Thread finalizado");
			threads--;
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
		Arguments *params;
		pthread_t tid[2];


		// Inicializacao do vetor de temperatura
		int i;
		for(i = 0; i < 2; i++){
			temperaturas[i] = 0;
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

			// Criacao da thread
			pthread_create(&(tid[threads++]), NULL, &response, (void *)params);
			pthread_detach(tid[threads]);
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
