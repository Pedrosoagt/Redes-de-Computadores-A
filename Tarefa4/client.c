#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio_ext.h>

#define NAME_SIZE 20
#define MSG_SIZE 80
#define ANS_SIZE 1250

struct msg{
	char name[NAME_SIZE];
	char message[MSG_SIZE];
	char answer[ANS_SIZE];
	int type;
};

/*
 * Cliente TCP
 */
void main(argc, argv)
int argc;
char **argv;
{
	unsigned short port;
	char sendbuf[12];
	char recvbuf[12];
	char msg[80];
	struct hostent *hostnm;
	struct sockaddr_in server;
	int s;
	int fin = 0;
	int type_clt=0;
	struct msg envio;

	/*
	 * O primeiro argumento (argv[1]) � o hostname do servidor.
	 * O segundo argumento (argv[2]) � a porta do servidor.
	 */
	if (argc != 3) {
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

	while(1) {

		// inicializacao padrao de valores da struct
		strcpy(envio.name, "empty");
		strcpy(envio.message, "empty");

		do{

			/*Menu Inicial*/
			puts("\n******************************");
			puts("* Opcoes:                    *");
			puts("*  1 - Cadastrar mensagem    *");
			puts("*  2 - Ler mensagens         *");
			puts("*  3 - Apagar mensagens      *");
			puts("*  4 - Sair                  *");
			puts("******************************");

			/*receber e tratar a selecao*/
			scanf("%i", &type_clt);

			switch(type_clt) {
				case 1:
					strcpy(msg, "");

					// Nome
					puts("\nUsuario (máximo 19 caracteres):");
					__fpurge(stdin);
					fgets(envio.name, NAME_SIZE, stdin);
					strtok(envio.name, "\n");
					// Mensagem
					puts("Mensagem (máximo 79 caracteres):");
					__fpurge(stdin);
					fgets(envio.message, MSG_SIZE, stdin);
					envio.type = 1;
				break;
				case 2:
					strcpy(msg, "\nMensagens cadastradas:");
					envio.type = 2;
				break;
				case 3:
					strcpy(msg, "\nMensagens removidas:");

					puts("\nUsuario:");
					__fpurge(stdin);
					fgets(envio.name, NAME_SIZE, stdin);
					strtok(envio.name, "\n");
					envio.type = 3;
				break;
				case 4:
					envio.type = 4;
					fin = 1;
				break;
				default:
					envio.type = 0;
					puts("\nColoque um valor válido!");
					__fpurge(stdin);
				break;
			}


		}while( !envio.type );


		/* Envia a mensagem no buffer de envio para o servidor */
		if (send(s, &envio, sizeof(envio), 0) < 0) {
			perror("Send()");
			exit(5);
		}


		/* Recebe a mensagem do servidor no buffer de recepcao */
		if (recv(s, &envio, sizeof(envio), 0) < 0) {
			perror("Recv()");
			exit(6);
		}

		// Finalizacao do programa
		if(envio.type == -1){
			close(s);
			puts("\nAte logo!");
			exit(0);
		}

		// print resultado:
		if(envio.type != 0)
			printf("%s %i\n\n", msg, envio.type);

		printf("%s\n", envio.answer);

		/*Limpando pra proxima*/
		type_clt = 0;
		strcpy(envio.name, "empty");
		strcpy(envio.message, "empty");
		strcpy(envio.answer, "\0");
	}

	/* Fecha o socket */
	close(s);

	printf("Cliente terminou com sucesso.\n");
	exit(0);
}
