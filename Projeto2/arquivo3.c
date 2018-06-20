#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


typedef struct lista{
    //int info;
    char name[10];   // x
    int num[100];  // Y
    struct lista* prox;
}Lista;

void write_file(char *type, char *name ,char  *stringNum) {

    FILE *fp;
    fp = fopen("contatos.txt","a+");

    if( fp )
      fprintf(fp, "%s-%s-%s\n", type, name, stringNum);
    

    printf("\n Numero armazenado:%s-%s\n", name, stringNum);
    fclose(fp);
}

void read_file(){
  FILE *fp;
  char *ans = NULL;
  char *line = NULL;
  size_t len = 0;

  // Aparentemente o endereço de ans quando inicializada
  //  como vetor fica mudando, então fiz por alocação
  // dinâmica mesmo.
  ans = (char *) malloc(1024);

  fp = fopen("contatos.txt", "r");

  if(fp)
    while ((getline(&line, &len, fp)) != EOF)
      strcat(ans, line);
    puts(ans);

  fclose(fp);
}



int main(argc, argv)

    int argc;
    char *argv[];
{

    char stringNum[100],stringNumAUX[10];
    char nomeGrupo[10];
    int op;
    int i;
    Lista *raiz;
    raiz = (Lista*)malloc(sizeof(Lista));
    raiz = NULL;
   
        printf("Digite a opção:\n1-Novo usuario\n2-Novo grupo\n3-Ler arquivo\n>:");
        scanf("%i",&op);
        switch(op)
        {
        case 1:
            printf("Digite o nome:");
            scanf("%s",nomeGrupo);
            printf("Digite o numero:");
            scanf("%s",stringNum);
            printf("numero pego:%s",stringNum);
            write_file("U",nomeGrupo,stringNum);
            break;
        case 2:
            i=0;
            int qtdnumeros = 0;           
            printf("Digite o nome do grupo:");
            scanf("%s",nomeGrupo);
            printf("Digite a quantidade de numeros:");
            scanf("%i",&qtdnumeros);

            do
            {
                scanf("%s",stringNumAUX);
                strcat(stringNum,stringNumAUX);
                strcat(stringNum,"|");
                //printf("string:%s",stringNum);
                i++;

            }while(i!=qtdnumeros);
            write_file("G",nomeGrupo,stringNum);
            break;
          case 3:
            
            read_file();
    

            

        default :
            break;
        }

}
