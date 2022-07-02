#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <ctype.h>

/*Figlio usa una exec*/

if (pid == 0) 
{
			/* codice del figlio */
			/*
			printf("Sono il processo figlio di indice %d e pid %d sto per leggere la prima linea del file %s\n", i, getpid(), argv[i+1]);
			*/
/* ogni figlio deve simulare il piping dei comandi nei confronti del padre e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
		close(1);
		dup(piped[i][1]); 			
		/* Chiusura di tutte le pipe */
		for (j=0; j < N; j++)
		{
			close(piped[j][0]);
			close(piped[j][1]);
		}
	/* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video)*/
		close(2);
		open("/dev/null", O_WRONLY);
				
	/* Il figlio diventa il comando head */				
		execlp("head", "head", "-1", argv[i+1], (char *)0);
				
	/* attenzione all'ordine dei parametri nella esecuzione di head: prima
-1 e poi nome del file e quindi terminatore della lista. Il file si trova usando
l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e'
argv[1]) */
				
		/* Non si dovrebbe mai tornare qui!!*/
		exit(1); /* torno un valore diverso da zero per indicare insuccesso*/
}

/* Codice del padre */
	for (i=0; i < N; i++)
		close(piped[i][1]);
	
	/* Il padre recupera le informazioni dai figli in ordine di indice */
	for (i=0; i < N; i++)
	{
		/* il padre recupera tutti i caratteri spediti da ogni figlio fino al carattere terminatore di linea '\n': ricordarsi che il figlio li scrive su standard output che pero' è stato collegato alla pipe che legge il padre */
		printf("Il figlio di indice %d ha letto dal file %s la linea:\n", i, argv[i+1]);
		do 
 		{
			read(piped[i][0], &ch, 1);
			printf("%c", ch);
		}
		while (ch != '\n');
	}

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore in wait\n");
		exit (5);
		}

		if ((status & 0xFF) != 0)
    		printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		printf("Il figlio con pid=%d ha ritornato %d\n", pid, (int)((status >> 8) &	0xFF));
	}








typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	/* controlliamo numero di parametri */
	/* devono esserci almeno tre parametri piÃ¹ argv[0] */
	if (argc < 4)
	{
	   printf("Errore nel numero di parametri: %d. Ci vuole almeno due nomi di file e un numero che rappresenta il numero medio di linee dei file!\n", argc);
	   exit(1);
	}

	/* calcoliamo il numero di files passati */
	N=argc-2;

	/* calcoliamo valore di K e lo controlliamo */
	K=atoi(argv[argc-1]);
	if (K < 0)
	{
	   printf("Errore valore di K\n");
	   exit(2);
	}

	/* allocazione pid */
	if ((pid=(int *)malloc(N*sizeof(int))) == NULL)
	{
	        printf("Errore allocazione pid\n");
	        exit(3);
	}

	/* allocazione dinamica pipe figli-padre */
	piped=(pipe_t *)malloc(sizeof(pipe_t)*N);
	if (piped == NULL)
	{
	   printf("Allocazione fallita pipe figli-padre\n");
	   exit(4);
	}
	/* creazione delle N pipe */
	for (i=0;i<N;i++)
	{
	   if (pipe(piped[i])!=0)
	   {
	      printf("Errore pipe\n");
	      exit(5);
	   }
	}

	/* ciclo per la generazione degli N figli */
	for (i=0;i<N;i++)
	{
	   pid[i]=fork();
	   if (pid[i]<0)  /* errore */
	   {
	      printf("Errore fork\n");
	      exit(7);
	   }
	   if (pid[i]==0)  /* figlio i-esimo */
	   {
		      /* ogni figlio NON legge da nessuna pipe e scrive solo sulla propria piped[i] */
		      for (j=0;j<N;j++)
		      {
		         close(piped[j][0]);
		         if(j!=i)
		         {
		            close(piped[j][1]);
		         }
	      	  }

	      	  /* apro il file di competenza, ossia quello di indice i+1 */
		      fd=open(argv[i+1],O_RDONLY);
		      if (fd < 0)
		      {
		         printf("Errore apertura file. Sono il figlio di indice %d\n",i);
		         exit(-1); /* in caso di errore decidiamo di tornare -1 che sara' interpretato come 255, che NON e' un valore accettabile per il padre */
		      }

		   //Dove ritorno è un valore che userò poi nel padre
		   exit(ritorno);

	   }
	}

	/* codice del padre */
	/* chiudo le pipe che non uso */
	for (i=0;i<N;i++)
	{
	   close(piped[i][1]);
	}

	/* riceviamo dai figli le lunghezze calcolate, in ordine di indice e quindi utilizzando queste lunghezze riceviamo le linee inviate dai figli */
	for (i=0;i<N;i++)
	{
	   nr=read(piped[i][0],&L,sizeof(int));
	   if (nr == 0)
		printf("Il figlio con indice %d e pid %d ha avuto dei problemi\n",  i, pid[i]);
	   else
	   {
	   read(piped[i][0],linea,L);
	   /* il padre trasforma la linea ricevuta in una stringa in modo da non avere problemi: sovrascriviamo lo \n tanto nella printf seguente lo si aggiunge */
	   linea[L-1]='\0';	
	   printf("Il figlio con indice %d e pid %d mi ha comunicato questo: %s\n", i, pid[i], linea); 
	   }
	}

	/* Attesa della terminazione dei figli */
	for(i=0;i<N;i++)
	{
	   pidFiglio = wait(&status);
	   if (pidFiglio < 0)
	   {
	      printf("Errore wait\n");
	      exit(8);
	   }
	   if ((status & 0xFF) != 0)
	   {
	   		printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
	   }           
	   else
	   {
	        ritorno=(int)((status >> 8) & 0xFF);
	        printf("Il figlio con pid=%d ha ritornato %d\n", pidFiglio, ritorno);
	   }
	}


	exit(0);


}