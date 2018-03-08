#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_SIZE 19
#define MG_SIZE 79

struct msg{
	char name[NAME_SIZE];
	char mensage[MG_SIZE];
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
     * O primeiro argumento (argv[1]) é o hostname do servidor.
     * O segundo argumento (argv[2]) é a porta do servidor.
     */
    if (argc != 3)
    {
        fprintf(stderr, "Use: %s hostname porta\n", argv[0]);
        exit(1);
    }

    /*
     * Obtendo o endereço IP do servidor
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }
    port = (unsigned short) atoi(argv[2]);

    /*
     * Define o endereço IP e a porta do servidor
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

    /* Estabelece conexão com o servidor */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connect()");
        exit(4);
    }
    /*Menu Inicial*/
    printf("Opções:\n 1-Cadastrar mensagem\n 2-Ler mensagens\n 3 - Apagar mensagens\n 4 - Sair da Aplicação\n");
    
    /*receber e tratar a seleção*/
    scanf("%i", &type_clt);
    
    switch(type_clt){
    	case 1:
		printf("Usuario:");
		scanf("%s", envio.name);
		printf("\nMensagem:");
		scanf("%s", envio.mensage);
		envio.type = 1;
    		break;
	case 2:
		strcpy(sendbuf, "Ler");
		break;
	case 3:
		strcpy(sendbuf, "Apagar");
		break;
	case 4:
		strcpy(sendbuf, "Sair");
		break;
	default:
		strcpy(sendbuf, "Requisição");
		break;
    }
    


    /* Envia a mensagem no buffer de envio para o servidor */
    if (send(s, &envio, sizeof(envio), 0) < 0)
    {
        perror("Send()");
        exit(5);
    }
    printf("Mensagem enviada ao servidor: %s\n", envio.mensage);

    /* Recebe a mensagem do servidor no buffer de recepção */
    if (recv(s, &envio, sizeof(envio), 0) < 0)
    {
        perror("Recv()");
        exit(6);
    }
    printf("Mensagem recebida do servidor: %s\n", envio.mensage);

    /* Fecha o socket */
    close(s);

    printf("Cliente terminou com sucesso.\n");
    exit(0);

}


