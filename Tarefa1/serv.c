#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 2000

void getOutput(char line[]){

	FILE *fp;
	char content[2000] = {0};
	char letter;
	int i;

	/* Open the command for reading. */
	fp = popen(line, "r");
	if (fp == NULL) {
	  printf("Failed to run command\n" );
	  exit(1);
	}

	i = 0;
	while(i < BUFF_SIZE) {
		line[i] = fgetc(fp);

		if(line[i] != EOF){
			if(line[i] == '\0')
				line[i] = '0';
			content[i] = line[i];
			i++;
		}
		else
			break;
	}

	/* close */
	pclose(fp);
	strcpy(line, content);
}


/* Server */
void main(argc, argv)
int argc;
char **argv;
{
	// S = Descritor do socket
	int s;

	// sockint = ?
	int sockint;

	// fin = comando de finalizacao vindo do cliente
	int fin = 0;

	// client_address_size = tamanho do endereco do client
	int client_address_size;

	// sockaddr_in = endereço do socket de entrada
	struct sockaddr_in client, server;

	// command = comando que sera utilizado
	char **command;

	char buf[BUFF_SIZE];

	int sendp=0;

	//printf("Dados resgatados:\nPorta: %s", argv[1]);
	printf("arg 0: %s\narg 1: %s\n", argv[0], argv[1]);

	// 1o passo Criar o socket UDP


	if( (s = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket()");
		exit(1);
	}

	// conversao argv string pra int
	int port = atoi(argv[1]);
	printf("port = %i\n", port);

	// 2o passo Definicao dos parametros de conexao

	server.sin_family		= AF_INET; 		// Selecao do tipo IPv4 (padrao)
	server.sin_port			= htons(port); 	// Selecao da porta de conexao (passado pela linha de comando)
	server.sin_addr.s_addr 		= INADDR_ANY; 	// Endereco ip do server

	// 3o ligar o server a porta definida
	if( bind(s, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind()");
		exit(1);
	}

	printf("Porta utilizada: %i\n", ntohs(server.sin_port));

	// 4o passo: escutar a porta para novas conexoes
	client_address_size = sizeof(client);

	puts("Log:");

	while(!fin){
		// Caso haja conexao, receber os dados
		if(recvfrom(s, buf, BUFF_SIZE, 0, (struct sockaddr *)&client, &client_address_size) < 0){
			perror("recvfrom()");
			exit(1);
		}

		printf("> %s\n", buf);

		//verificar se nao eh uma finalizacao
		if(strcmp(buf, "exit") == 0){
			fin = 1;
			strcpy(buf, "exit");
		}else{
			getOutput(buf);
		}

		int i = 0;

			sendp = sendto(s, buf, BUFF_SIZE, 0, (struct sockaddr *)&client, sizeof client );
			//printf("%i\n", countsp);
			//5o passo: enviar para o cliente
			if(sendp < 0){
				perror("sendto()");
				exit(1);
			}
	}

	close(s);
}
