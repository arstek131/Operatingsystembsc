/*
Scrivere un applicativo che crei due processi P1 e P2.
P1 prende una stringa e la passa a P2.
P2 concatena la stringa ricevuta con un'altra stringa e la manda indietro a P1.
Infine P1 stampa la stringa ricevuta.

Altra stringa = "ogle.it"
Input stringa= "www.go"

OUTPUT="www.google.it"
*/

#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  

int main(int argc, char **argv)
{
	//Uso due pipe
	//1 Pipe --> per inviare la stringa dal padre
	//2 Pipe --> per inviare la stringa concatenata dal figlio

	int fd1[2];
	int fd2[2];

	char mia_stringa[]="ogle.it";
	char input_string[100];

	pid_t p;

	if(pipe(fd1)==-1)
	{
		printf("La pipe 1 ha fallito\n");
		exit(-1);
	}
	if(pipe(fd2)==-1)
	{
		printf("La pipe 2 ha fallito\n");
		exit(-1);
	}

	printf("Inserisci la stringa da concatenare: ");
	scanf("%s",input_string);

	p=fork();

	if(p<0)
	{
		printf("\nLa fork ha fallito\n");
		exit(-2);
	}

	//caso del padre
	else if(p > 0) 
	{
		char concat_str[100];
		close(fd1[0]); //chiudo la lettura della prima pipe

		//Scrivo la stringa nella pipe, e chiudo la scrittura
		write(fd1[1],input_string,strlen(input_string)+1);
		close(fd1[1]);

		//Aspetto che il figlio mandi qualcoa
		wait(NULL);

		close(fd2[1]); //chiudo la scrittura della seconda pipe

		//Leggo la stringa dal figlio, la stampo e chiudo la lettura
		read(fd2[0],concat_str,100);
		printf("La stringa concatenata %s\n",concat_str);
		close(fd2[0]);
	}
	//caso del figlio
	else
	{
		close(fd1[1]);//chiudo la scrittura della 1 pipe

		//Leggo la stringa usando la prima pipe
		char concat_str[100];
		read(fd1[0],concat_str,100);

		int k=strlen(concat_str);
		int i;

		for (i=0; i<strlen(mia_stringa); i++) 
            concat_str[k++] = mia_stringa[i]; 
  
        concat_str[k] = '\0';

        //chiudo la lettura in entrambe le pipe
        close(fd1[0]);
        close(fd2[0]);

        //Scrivo la stringa concatenata e chiudo la scrittura
        write(fd2[1],concat_str,strlen(concat_str)+1);
        close(fd2[1]);

        exit(0);

	}




}