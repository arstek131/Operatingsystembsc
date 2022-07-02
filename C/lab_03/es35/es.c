#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 


typedef struct
{
	char val1;
	int id;
	int value;
	
}s_occ;


int main(int argc, char **argv)
{
	int nFigli;
	int p[2];
	int i;
	int pid;
	int nr,nw;

	//controllo il numero di argomenti
	if(argc != 2)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(EXIT_FAILURE);
	}

	//Mi salvo il numero dei figli
	nFigli=atoi(argv[1]);
	if(nFigli <= 0)
	{
		printf("Numero di figli deve essere >= 1\n");
		exit(EXIT_FAILURE);
	}

	//Creo la pipe
	if(pipe(p)!=0)
	{
		printf("Errore nella creazione della pipe()\n");
		exit(EXIT_FAILURE);
	}

	//Comincio a creare gli N figli
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella invocazione della fork()\n");
			exit(EXIT_FAILURE);
		}
		//caso del figlio
		else if(pid == 0)
		{
			s_occ tmp2;
			printf("Il figlio %d ha cominciato\n",getpid());

			//chiudo la lettura, siccome il figlio ha solo il compito di scriverci
			close(p[0]);

			//Riempio la struct
			tmp2.val1='a';
			tmp2.id=i;
			tmp2.value=15+i;

			printf("Il figlio %d sta scrivendo nella pipe..\n",i);

			nw=write(p[1],&tmp2,sizeof(tmp2));

			if(nw != sizeof(tmp2))
			{
				printf("Errore nella scrittura sulla pipe da parte del figlio %d\n",i);
				exit(EXIT_FAILURE);
			}

			exit(EXIT_SUCCESS);


		}
	}

		//caso del padre
		if(pid > 0)
		{
			s_occ tmp;

			printf("Sono il padre %d e sono partito\n",getpid());

			//Chiudo la pipe in scrittura (siccome il padre ha solo il compito di leggere)
			close(p[1]);


			for(i=0; i<nFigli; i++)
			{
				nr=read(p[0],&tmp,sizeof(tmp));
				if(nr != sizeof(tmp))
				{
					printf("Errore nella lettura da parte del padre\n");
					exit(EXIT_FAILURE);
				}
				printf("Il padre ha letto correttamente la struttura dal %d figlio e ora la stampo...\n",i);
				printf("Primo campo: %c\n Secondo campo: %d\n Terzo campo: %d\n",tmp.val1,tmp.id,tmp.value);
			}

			for(i=0; i<nFigli; i++)
			{
				wait(NULL);
			}

			exit(EXIT_SUCCESS);
			


		}












}