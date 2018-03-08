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
#define ANS_SIZE 1170

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
    struct hostent *hostnm;
    struct sockaddr_in server;
    int s;
    int type_clt=0;
    struct msg envio;

    /*
     * O primeiro argumento (argv[1]) � o hostname do servidor.
     * O segundo argumento (argv[2]) � a porta do servidor.
     */
    if (argc != 3)
    {
        fprintf(stderr, "Use: %s hostname porta\n", argv[0]);
        exit(1);
    }

    /*
     * Obtendo o endere�o IP do servidor
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0)
    {
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
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket()");
        exit(3);
    }

    /* Estabelece conex�o com o servidor */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect()");
        exit(4);
    }

		while(1){
	    /*Menu Inicial*/
	    printf("Opcoes:\n 1-Cadastrar mensagem\n 2-Ler mensagens\n 3 - Apagar mensagens\n 4 - Sair da Aplica��o\n");

	    /*receber e tratar a sele��o*/
	    scanf("%i", &type_clt);

			// inicializacao padrao de valores da struct
			strcpy(envio.name, "empty");
			strcpy(envio.message, "empty");

	    switch(type_clt){

				case 1:
					printf("Usuario (máximo 19 caracteres):\n");
					__fpurge(stdin);
					fgets(envio.name, NAME_SIZE, stdin);
					printf("Mensagem (máximo 79 caracteres):\n");
					__fpurge(stdin);
					fgets(envio.message, MSG_SIZE, stdin);
					envio.type = 1;
	    		break;

				case 2:
					envio.type = 2;
					break;

				case 3:
					envio.type = 3;
					break;

				case 4:
					envio.type = 0;
					break;

				default:
					puts("Coloque um valor válido!");
					__fpurge(stdin);
					break;
	    }

			// Finalizacao do programa
			if(!envio.type){
				close(s);
				puts("Ate logo!");
				exit(0);
			}

	    /* Envia a mensagem no buffer de envio para o servidor */
	    if (send(s, &envio, sizeof(envio), 0) < 0)
	    {
	        perror("Send()");
	        exit(5);
	    }
	    printf("Mensagem enviada ao servidor: %i\n", envio.type);

	    /* Recebe a mensagem do servidor no buffer de recep��o */
	    if (recv(s, &envio, sizeof(envio), 0) < 0)
	    {
	        perror("Recv()");
	        exit(6);
	    }
	    printf("Mensagem recebida do servidor: %s\n", envio.answer);
		}
    /* Fecha o socket */
    close(s);

    printf("Cliente terminou com sucesso.\n");
    exit(0);

}
