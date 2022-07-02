Il programma, per ognuna delle N fasi, deve esplorare la gerarchia Gi specificata - tramite un file comandi
ricorsivo, FCR.sh - e deve contare globalmente per ogni gerarchia Gi tutti i file leggibili che soddisfano la
seguente specifica: il contenuto del file deve essere tale per cui tutte le sue linee iniziano con il carattere ‘a’. Al
termine di tutte le N fasi, per ognuna delle gerarchie Gi, si deve riportare sullo standard output il nome della
gerarchia Gi e il numero totale di file di quella gerarchia che soddisfano la specifica precedente (file trovati);
inoltre, si deve riportare sullo standard output il nome assoluto di ogni file trovato chiedendo contestualmente
all’utente un numero X: tale numero deve essere usato per riportare sullo standard output le prime X linee del
file trovato corrente.



*********+FILE P**************
n=1 #serve per dare un nome diverso ai file temporanei e volendo per dire in che fase siamo
for i
do
	> /tmp/conta$$-$n #creaimo/azzeriamo il file temporaneo
	echo fase $n
	#invochiamo il file comandi ricorsivo con la gerarchia e il file temporaneo
	FCR.sh $i /tmp/conta$$-$n
	n=`expr $n + 1`
done

#terminate tutte le ricerche ricorsive cioè le N fasi
n=1 #serve per dare un nome diverso ai file temporanei e volendo per dire in che fase siamo
for i
do
	#N.B. Andiamo a contare le linee del file /tmp/conta$$-$n
	F=`wc -l < /tmp/conta$$-$n`
	echo Il numero di file totali della gerarchia $i che soddisfano la specifica = $F
	#echo i file trovati sono
	#cat  /tmp/conta$$-$n
	for j in `cat  /tmp/conta$$-$n`
	do
		#Stampiamo (come richiesto dal testo) i nomi assoluti dei file trovati
		echo Trovato il file $j
		#chiediamo all'utente il numero X
		echo -n "Dammi il numero X "
		read X
		#Controllo X
		case $X in
		*[!0-9]*) echo non numerico o non positivo
				exit 4;;
		*) ;;
		esac
		#selezioniamo direttamente le prime $x linee del file corrente
		head -$X < $j
	done
	#da ultimo eliminiamo il file temporaneo
	rm /tmp/conta$$-$n
	n=`expr $n + 1`
done

****************FILE R*************************
#!/bin/sh
#FCR.sh
#file comandi ricorsivo che scrive il nome dei direttori trovati sul file
#il cui nome e' passato come secondo argomento

cd $1
#la variabile NR ci serve per il numero di linee
NR=
#la variabile NG ci serve per il numero di linee trovate dal grep
NG=

for i in *
do
	#controlliamo solo i nomi dei file leggibili!
	if test -f $i -a -r $i
	then
		NR=`wc -l < $i`
		if test $NR -ne 0
		then
			NG=`grep '^a' $i | wc -l`
			if test $NR -eq $NG
			then
			#abbiamo trovato un file che soddisfa le specifiche e quindi inseriamo il suo nome assoluto nel file temporaneo
			#echo TROVATO FILE `pwd`/$i
			echo `pwd`/$i >> $2
			fi
		fi
	fi
done
for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva
		FCR.sh `pwd`/$i $2
	fi
done





