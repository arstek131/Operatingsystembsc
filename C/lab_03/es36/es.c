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

typedef int pipe_t[2];

int main(int argc, char **argv)
{
	int nw,nr;
	s_occ tmp,tmp2;
	int pid;
	int i,j;
	int nFigli;
	pipe_t *p; //variabile usata per crearmi l'array di pipe (una per ogni figlio)

	//controllo il numero di argomenti
	if(argc != 2)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(EXIT_FAILURE);
	}

	nFigli=atoi(argv[1]);
	if(nFigli <= 0)
	{
		printf("Per favore un numero > 0\n");
		exit(EXIT_FAILURE);
	}

	//Alloco la memoria per le N pipe, siccome devo creare una pipe per ogni figlio
	p = (pipe_t *) malloc(sizeof(pipe_t)*nFigli);
	if(p==NULL)
	{
		printf("Allocazione delle N pipe fallita\n");
		exit(EXIT_FAILURE);
	}

	//Inizializzo ogni pipe (ovvero invocando su ognuna di essa il metodo pipe())
	for(i=0; i<nFigli; i++)
	{
		if(pipe(p[i]) < 0)
		{
			printf("Errore nella invocazione della funzione pipe()\n");
			exit(EXIT_FAILURE);
		}
	}

	//Faccio la fork per nFigli con un for e valuto i casi (padre o figlio)
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella invocazione del metodo fork()\n");
			exit(EXIT_FAILURE);
		}
		//caso del figlio
		else if(pid == 0)
		{
			printf("Il figlio %d ha cominciato..\n",getpid());

			//Chiudo le pipe che non uso, ovvero..
			//Siccome il padre ha il compito solo di leggere dalle pipe, quindi il figlio scrivere
			//Allora a TUTTI i figli chiudiamo la lettura
			//E a tutti tranne quello preso in esame anche la scrittura

			for(j=0; j<nFigli; j++)
			{
				close(p[j][0]);
				if(i != j) close(p[j][1]);
				
			}

			//riempio la struct
			tmp.val1='a';
			tmp.id=i;
			tmp.value=i+20;
			
			//Scrivo la struct nella pipe
			printf("Il figlio %d con pid %d sta per scrivere nella pipe\n",i,getpid());
			nw=write(p[i][1],&tmp,sizeof(tmp));
			if(nw != sizeof(tmp))
			{
				printf("Erore nella scrittura sulla pipe da parte del figlio %d\n",i);
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);

		}
	}

	//codice del padre
	if(pid > 0 )
	{

		printf("Il padre %d ha cominciato..\n",getpid());

		//Siccome il padre ha il solo compito di leggere
		//Chiudo la scrittura verso tutti i figli
		for(i=0; i<nFigli; i++)
		{
			close(p[i][1]);
		}

		//La consegna mi chiede di leggerle al contrario.. 
		//Quindi faccio un ciclo che parte al contrario
		for(i=nFigli-1; i>=0; i--)
		{
			nr=read(p[i][0],&tmp2,sizeof(tmp2));
			if(nr != sizeof(tmp2))
			{
				printf("Errore nella lettura dalla pipe da parte del padre\n");
				exit(EXIT_FAILURE);
			}
			printf("Il padre ha letto correttamente la struttura dal figlio e ora la stampo...\n");
			printf("Primo campo: %c\n Secondo campo: %d\n Terzo campo: %d\n",tmp2.val1,tmp2.id,tmp2.value);
		}


		//Aspetto i vari figli..
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(EXIT_SUCCESS);

	}







}