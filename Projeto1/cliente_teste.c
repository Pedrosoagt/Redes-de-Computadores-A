#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(argc, argv)
int argc;
char **argv;
{
  unsigned short port;
  struct hostent *hostnm;
  struct sockaddr_in server;
  int s;
	int type_clt=0;
  int status = -1;
  float temperatura = 0;

  /*
	 * O primeiro argumento (argv[1]) � o hostname do servidor.
	 * O segundo argumento (argv[2]) � a porta do servidor.
	 */
  if(argc != 3){
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

  printf("Digite Uma temperatura\n");
  scanf("%f", &temperatura);

  /*----------------Envio-----------------------------*/
  if (send(s, &temperatura, sizeof(float), 0) &&  (send(s, &status, sizeof(int), 0)) < 0) {
    perror("Send()");
    exit(5);
  }

  /* Fecha o socket */
	close(s);

	printf("Cliente terminou com sucesso.\n");
	exit(0);
}
