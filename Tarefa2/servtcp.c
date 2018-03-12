#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

/* Constantes do programa */
#define NAME_SIZE 20
#define MSG_SIZE 80
#define LIST_SIZE 10
#define ANS_SIZE 1250

/*
* Struct to save Mensagens
*/

typedef struct Msg {
	char name [NAME_SIZE];
	char message [MSG_SIZE];
	char answer [ANS_SIZE];
	int type;
}Msg;

//funções auxiliares

//Procura e armazena nova msg
int insertMsg(Msg list[], Msg *envio){

	int i;

	// procurando espaco vazio e atribuindo nova msg
	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, "") == 0){
			list[i] = *envio;
			strcpy(envio->answer, "Mensagem incluida com sucesso!");
			envio->type = 0;
			return 1;
		}
	}

	strcpy(envio->answer, "A mensagem nao foi incluida. A lista ja esta cheia!");
	return 0;
}

int removeMsg(Msg list[], int pList, Msg *envio){
	
	int i, rmvdMsgs = 0;
	char res[1200] = "";
	
	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, envio->name) == 0){
			
			// salvando valores
			strcat(res, "Usuario:");
			strcat(res, list[i].name);
			strcat(res, " | ");
			strcat(res, "Mensagem:");
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

int getMsgs(Msg list[], int pList, Msg *envio){

	char res[ANS_SIZE] = {""};
	int i, count = 0;

	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, "\0") != 0){
			strcat(res, "Usuario: ");
			strcat(res, list[i].name);
			strcat(res, "  |  ");
			strcat(res, "Mensagem: ");
			strcat(res, list[i].message);
			count++;
		}
	}
	
	envio->type = count;
	strcpy(envio->answer, res);
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
    int s;			/* Socket para aceitar conex�es       */
    int ns;			/* Socket conectado ao cliente        */
    int namelen;
    Msg list[10] = {"\0"};	/* list of mensagens			*/
    int pList = 0;
    Msg envio;
    /*
     * O primeiro argumento (argv[1]) � a porta
     * onde o servidor aguardar� por conex�es
     */
    if (argc != 2) {
        fprintf(stderr, "Use: %s porta\n", argv[0]);
        exit(1);
    }

    port = (unsigned short) atoi(argv[1]);

    /*
     * Cria um socket TCP (stream) para aguardar conex�es
     */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket()");
        exit(2);
    }

   /*
    * Define a qual endere�o IP e porta o servidor estar� ligado.
    * IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
    * os endere�os IP
    */
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    /*
     * Liga o servidor � porta definida anteriormente.
     */
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
       perror("Bind()");
       exit(3);
    }

    /*
     * Prepara o socket para aguardar por conex�es e
     * cria uma fila de conex�es pendentes.
     */
    if (listen(s, 1) != 0) {
        perror("Listen()");
        exit(4);
    }

    /*
     * Aceita uma conex�o e cria um novo socket atrav�s do qual
     * ocorrer� a comunica��o com o cliente.
     */
    namelen = sizeof(client);
    if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1) {
        perror("Accept()");
        exit(5);
    }

	while(1) {
	    /* Recebe uma mensagem do cliente atrav�s do novo socket conectado */
	    if (recv(ns, &envio, sizeof(envio), 0) == -1) {
	        perror("Recv()");
	        exit(6);
	    }
	    puts("Mensagem recebida do cliente:");
		printf("Tipo: %i | Nome: %s | Msg: %s\n", envio.type, envio.name, envio.message);

		switch(envio.type) {
			case 1:
				if(insertMsg(list, &envio) == 1);
					pList++;	
			break;
			case 2:
				getMsgs(list, pList, &envio);
			break;
			case 3:
				pList -= removeMsg(list, pList, &envio);
			break;
		}

	    /* Envia uma mensagem ao cliente atrav�s do socket conectado */
	    if (send(ns, &envio, sizeof(envio), 0) < 0) {
	        perror("Send()");
	        exit(7);
	    }
	    printf("Mensagem enviada ao cliente: %s\n", envio.answer);
	    /*Limpando pra proxima*/
	    strcpy(envio.name, " ");
	    strcpy(envio.message, " ");
	    strcpy(envio.answer, " ");
	    envio.type = 0;
	}

    /* Fecha o socket conectado ao cliente */
    close(ns);

    /* Fecha o socket aguardando por conex�es */
    close(s);

    printf("Servidor terminou com sucesso.\n");
    exit(0);
}
