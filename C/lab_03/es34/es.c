#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

typedef int pipe_t[2];

typedef struct
{
	char val1;
	int id;
	int value;
	
}s_occ;


int main(int argc, char **argv)
{
	int pid;
	s_occ tmp;
	int p[2];
	int nw,nr;

	if(pipe(p)<0)
	{
		printf("Errore nella pipe()\n");
		exit(1);
	}

	if((pid=fork())<0)
	{
		printf("Errore nella fork()\n");
		exit(2);
	}
	//caso del figlio
	else if(pid==0)
	{
		s_occ tmp2;

		printf("Il figlio %d ha cominciato\n",getpid());

		//chiudo la pipe in scrittura, siccome il figlio ha solo il compito di leggere, mentre il padre di scrivere
		close(p[1]);

		//leggo con una read
		nr=read(p[0],&tmp2,sizeof(tmp2));
		if(nr!=sizeof(tmp2))
		{
			printf("Errore nella lettura da parte del figlio\n");
			exit(EXIT_FAILURE);
		}

		printf("La lettura da parte del figlio e' avvenuta con successo\n");
		printf("Ora stampo i valori ricevuti...\n");
		printf("Primo campo: %c\n Secondo campo: %d\n Terzo campo: %d\n",tmp2.val1,tmp2.id,tmp2.value);

		exit(EXIT_SUCCESS);

	}
	//caso del padre
	if(pid>0)
	{
		printf("Il padre %d ha cominciato\n",getpid());

		//chiudo la pipe dalla parte della lettura, siccome il padre deve solo scrivere, mentre il figli deve leggere
		close(p[0]);

		//riempio la struct con i valori;

		tmp.val1='a';
		tmp.id=1;
		tmp.value=15;

		//scrivo nella pipe
		nw=write(p[1],&tmp,sizeof(tmp));
		if(nw != sizeof(tmp))
		{
			printf("Errore nella scrittura da parte del padre\n");
			exit(EXIT_FAILURE);
		}

		printf("La scrittura da parte del padre e' avvenuta con successo\n");

		wait(NULL);
		exit(EXIT_SUCCESS);


	}





}