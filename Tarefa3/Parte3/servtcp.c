#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

/* Constantes do programa */
#define NAME_SIZE 20
#define MSG_SIZE 80
#define LIST_SIZE 10
#define ANS_SIZE 1250
#define SHM_KEY 9018

pthread_mutex_t mutex;

/*
* Struct to save Mensagens
*/

typedef struct {
	char name [NAME_SIZE];
	char message [MSG_SIZE];
	char answer [ANS_SIZE];
	int type;
}Msg;

typedef struct {
	Msg list[10];
} Shared;

//funções auxiliares

/*Funções para o mutex*/

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


/* Funções para controle de mensagens */

//Procura e armazena nova msg
int insertMsg(Msg list[], Msg *envio){
	lockMutex();
	int i;

	// procurando espaco vazio e atribuindo nova msg
	for(i = 0; i < LIST_SIZE; i++){
		if(strcmp(list[i].name, "") == 0){
			list[i] = *envio;
			strcpy(envio->answer, "\nMensagem incluida com sucesso!");
			envio->type = 0;
			unlockMutex();
			return 1;
		}
	}

	strcpy(envio->answer, "A mensagem nao foi incluida. A lista ja esta cheia!");
	unlockMutex();
	return 0;
}

int removeMsg(Msg list[], int pList, Msg *envio){
	lockMutex();

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
		unlockMutex();
	}

	strcpy(envio->answer, res);
	if(rmvdMsgs == 0)
		strcat(envio->answer, "Nao existem mensagens desse usuario.");
	envio->type = rmvdMsgs;
	unlockMutex();
	return rmvdMsgs;
}

int getMsgs(Msg list[], int pList, Msg *envio) {
	lockMutex();
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
		unlockMutex();
	}

	envio->type = count;
	strcpy(envio->answer, res);
	unlockMutex();
}

/* Funccoes para controle de processos */
void killZombie(){

	int status = 0;
	while(waitpid(-1, &status, WNOHANG) > 0);	//kill de um pid determinado
}


/* Memoria compartilhada */


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
    int s;					/* Socket para aceitar conexoes       */
    int ns;					/* Socket conectado ao cliente        */
    int namelen;		/* tamanho do nome */
		Msg *list = NULL;			/* lista de mensagens		*/
		Shared *shm = NULL;
    int pList = 0;	/* numero de mensagens cadastradas */
    Msg envio;			/* struct que sera enviada */

		// Mapeamento da memoria compartilhada
		int mem_id = shmget(IPC_PRIVATE, sizeof(Shared), SHM_R | SHM_W);


		// Inicializacao do mutex para memoria compartilhada
    inicializaMutex();

		pid_t pid, fid;

		// Inicializacao do vetor de mensagens
		int i;
		shm = (Shared *) shmat(mem_id, NULL, 0);
		list = shm->list;
		for(i = 0; i < LIST_SIZE; i++){
			strcpy(list[i].name, "");
			strcpy(list[i].message, "");
		}

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

		// tratamento para processos zumbis
		signal(SIGCHLD, killZombie);

		/*loop do Fork*/
		while (1) {
			/*
	     * Aceita uma conex�o e cria um novo socket atrav�s do qual
	     * ocorrer� a comunica��o com o cliente.
	     */

	    namelen = sizeof(client);
	    if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1) {
	        perror("Accept()");
	        exit(5);
	    }

			if ((pid = fork()) == 0) {
				/*Processo filho*/

				shm = (Shared *) shmat(mem_id, NULL, 0);
				list = shm->list;
				if ((void *)list == (void *)-1) exit(1);

				/* Fecha o socket aguardando por conex�es */
				close(s);

				/* Processo filho obtem seu pr�prio pid */
				fid = getpid();

				while(1) {
			    /* Recebe uma mensagem do cliente atrav�s do novo socket conectado */
			    if (recv(ns, &envio, sizeof(envio), 0) == -1) {
			        perror("Recv()");
			        exit(6);
			    }

					puts("Mensagem recebida do cliente:\n");
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
			}/*fim do if filho*/
			else{
				/*Processo pai*/
				if (pid > 0) {
			    printf("Processo filho criado: %d\n", pid);

			    /* Fecha o socket conectado ao cliente */
			    close(ns);
				}
				else {
				    perror("Fork()");
				    exit(7);
				}
			}/*fim do else pai*/
		}/*Fim do loop fork*/
	finalizMutex();
	shmdt(list);
}
