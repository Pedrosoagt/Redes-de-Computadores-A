#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>



/*
 * Servidor TCP
 */
int main(argc, argv)
int argc;
char **argv;
{
    unsigned short port;
    char sendbuf[12];
    char recvbuf[12];
    struct sockaddr_in client;
    struct sockaddr_in server;
    int s;                     /* Socket para aceitar conexões       */
    int ns;                    /* Socket conectado ao cliente        */
    int namelen;
    pid_t pid, fid;
    void killzombie();

    /*
     * O primeiro argumento (argv[1]) é a porta
     * onde o servidor aguardará por conexões
     */
    if (argc != 2)
    {
	  fprintf(stderr, "Use: %s porta\n", argv[0]);
	  exit(1);
    }

    port = (unsigned short) atoi(argv[1]);

    /*
     * Cria um socket TCP (stream) para aguardar conexões
     */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
	  perror("Socket()");
	  exit(2);
    }

   /*
    * Define a qual endereço IP e porta o servidor estará ligado.
    * IP = INADDDR_ANY -> faz com que o servidor se ligue em todos
    * os endereços IP
    */
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    /*
     * Liga o servidor à porta definida anteriormente.
     */
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
	perror("Bind()");
	exit(3);
    }

    /*
     * Prepara o socket para aguardar por conexões e
     * cria uma fila de conexões pendentes.
     */
    if (listen(s, 1) != 0)
    {
	  perror("Listen()");
	  exit(4);
    }

    signal(SIGCHLD, killZombie);//chama a função apenas se o cliente filho morrer
    while(1)
    {
	  /*
	  * Aceita uma conexão e cria um novo socket através do qual
	  * ocorrerá a comunicação com o cliente.
	  */
	  namelen = sizeof(client);
	  if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
	  {
		perror("Accept()");
		exit(5);
	  }

	  if ((pid = fork()) == 0) {
		/*
		 * Processo filho
		 */

		/* Fecha o socket aguardando por conexões */
		close(s);

		/* Processo filho obtem seu próprio pid */
		fid = getpid();

		/* Recebe uma mensagem do cliente através do novo socket conectado */
		if (recv(ns, recvbuf, sizeof(recvbuf), 0) == -1)
		{
		    perror("Recv()");
		    exit(6);
		}

		printf("[%d] Recebida a mensagem do endereço IP %s da porta %d\n", fid, inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		printf("[%d] Mensagem recebida do cliente: %s\n", fid, recvbuf);

		printf("[%d] Aguardando 10 s ...\n", fid);
		sleep(10);

		strcpy(sendbuf, "Resposta");

		/* Envia uma mensagem ao cliente através do socket conectado */
		if (send(ns, sendbuf, strlen(sendbuf)+1, 0) < 0)
		{
		    perror("Send()");
		    exit(7);
		}
		printf("[%d] Mensagem enviada ao cliente: %s\n", fid, sendbuf);

		/* Fecha o socket conectado ao cliente */
		close(ns);

		/* Processo filho termina sua execução */
		printf("[%d] Processo filho terminado com sucesso.\n", fid);
		exit(0);
	  }
	  else
	  {
		/*
		 * Processo pai
		 */

		if (pid > 0)
		{
		    printf("Processo filho criado: %d\n", pid);

		    /* Fecha o socket conectado ao cliente */
		    close(ns);
		}
		else
		{
		    perror("Fork()");
		    exit(7);
		}
	  }
    }


}
void killZombie(){

	int status = 0;
	while(waitpid(-1,&status,WNOHANG)>0);//kill de um pid determinado
}
