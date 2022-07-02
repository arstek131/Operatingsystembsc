/*
Scrivere un applicativo che crei due processi P1 e P2.
P1 prende un array di interi e lo passi a P2.
P2 lasci nell'array solo i numeri primi, mettendo 0 quando un numero non è primo, e rimandi l'array indietro a P1.
Infine P1 stampa l'array.


*/


#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

#define N 7

int checkprime(int number)
{
	int i;
    for (i=2; i<number; i++) {
        if (number % i == 0 && i != number) return 1;
    }
    return 0;

}
  

int main(int argc, char **argv)
{
	

	int myArr[N]={3,9,17,21,4,2,5};

	printf("\nStampo l'array senza modifiche");
	int u;
	for(u=0; u<N; u++)
	{
		printf("\t%d",myArr[u]);
	}

	//mi dichiaro 2 pipe
	int fd1[2];
	int fd2[2];

	pid_t p;

	if(pipe(fd1)==-1)
	{
		printf("La pipe ha fallito\n");
		exit(-1);
	}
	if(pipe(fd2)==-1)
	{
		printf("La pipe ha fallito\n");
		exit(-1);
	}

	p=fork();

	if(p<0)
	{
		printf("\nLa fork() ha fallito\n");
		exit(-2);
	}

	//caso del padre
	if(p>0)
	{
		int i;
		int printArr[N];

		close(fd1[0]);

		write(fd1[1],myArr,N*sizeof(int));
		
		
		close(fd1[1]);

		//Ora aspetto che il figlio mi mandi qualcosa
		wait(NULL);

		close(fd2[1]);
		read(fd2[0],printArr,N*sizeof(int));
		
		printf("\nStampo l'array modificato: ");
		for(i=0; i<N; i++)
		{
			printf("\t%d",printArr[i]);
		}

		close(fd2[0]);


	}
	//caso del figlio
	else if(p==0)
	{
		int i,k;
		close(fd1[1]);
		int readArr[N];

		read(fd1[0],readArr,N*sizeof(int));
		

		for(i=0; i<N; i++)
		{
			k=checkprime(readArr[i]);
			if(k==1)
				readArr[i]=0;
		}

		close(fd1[0]);
		close(fd2[0]);

		write(fd2[1],readArr,N*sizeof(int));
		close(fd2[1]);

		exit(0);



		


	}













}