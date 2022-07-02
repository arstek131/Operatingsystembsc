#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	int L;
	char linea[255];
	int i,j;
	int fd;
	int X;
	int trovata

	printf("Inserire un numero intero strettamente positivo X\n");
	scanf("%d",&X);
	/* controlliamo X */
	if (X <= 0)
	{
	   printf("Il valore X inserito non e' valido\n");
	   exit(6);
	}

	fd=open(argv[i+1],O_RDONLY);
	if(fd < 0)
	{
		printf("Errore apertura file\n");
		exit(-1);
	}

	L=0;
	j=1;
	while(read(fd,&(linea[L]),1)!=0)
	{
		if(linea[L] == '\n')
		{
			if(j==X)
			{
				L++;

			}
		}
		//printf("\n******Debug line %d******\n",L);
	}

	printf("HO FINITO\n");


	return 0;
}