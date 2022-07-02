#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 3; then
  echo Numero paremetri sbagliato
  exit 1
fi

#controllo che si tratti di un numero e che sia >0
expr $1 + 0  > /dev/null 2>&1
if test $? -ne 2 -a $? -ne 3
  then
     if test $1 -le 0
       then echo $1 non positivo
          exit 2
     fi
else
  echo $1 non numerico
  exit 3
fi

#controllo che si tratti di una directory e che sia assoluta
for i
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non directory
	    exit 4
	    fi;;
	*)  echo $i non nome assoluto; exit 5;;
	esac
done

#Controllo che si tratti di un nome relativo semplice
case $1 in
*/*) echo Errore: $1 non in forma relativa semplice
    exit 2;;
*) ;;
esac

#controllo si tratti di un singolo carattere
case $1 in
?);;
*)
    echo "Parametro $1 non e' un singolo carattere"
    exit 2;;
esac

#controllo si tratti di un singolo carattere numerico
case $2 in
  [0-9]) ;;
  *) echo Non singolo carattere numerico
     exit 4
     ;;
esac


#puo' avere senso controllare che il secondo parametro non contenga un carattere /
case $2 in
*/*) echo Errore: $2 non in forma relativa semplice
     exit 5;;
*) ;;
esac

##########################################################################
y=$1 #salvataggio del parametro
shift #quindi ora $1 diventa $2 (perdo il primo e slitta verso sx <-)

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

> /tmp/conta$$ #creiamo/azzeriamo il file temporaneo.

for i
do
	echo fase per $i
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR.sh $i $X /tmp/conta$$
done


#terminate tutte le ricerche ricorsive cioè le N fasi
#N.B. Andiamo a contare le linee del file /tmp/conta$$
F=`wc -l < /tmp/conta$$`

echo Il numero di file totali che soddisfano la specifica = $F

for i in `cat /tmp/conta$$`
do
        #Stampiamo (come richiesto dal testo) i nomi assoluti dei file creati
        echo Creato il file $i
        case $i in
        *NO*) echo il file originale non conteneva almeno 5 linee e quindi il file $i e\' vuoto ;;
        *)    echo il contenuto del file $i e\' il seguente\:
              cat < $i ;;
        esac
done
#da ultimo eliminiamo il file temporaneo
rm /tmp/conta$$



exit 0
