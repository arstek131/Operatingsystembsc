#!/bin/sh


#controllo il numero dei parametri
if test $# -le 1; then
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

tmp=$1
shift

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

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

> /tmp/conta$$

for i
do
	echo fase per $i
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR.sh $i $tmp /tmp/conta$$
done

x=0
num_li=`wc -l < /tmp/conta$$`
echo Il numero di file trovati globalmente e $num_li

echo FASE B
for i in `cat /tmp/conta$$`
do
  echo FILE $i
  echo Inserisci un numero x compreso tra 1 e minore di $tmp
  read x
    #controllo che si tratti di un numero e che sia >0
    expr $x + 0  > /dev/null 2>&1
    if test $? -ne 2 -a $? -ne 3
    then
      if test $x -le 0 -a $x -gt $tmp
       then echo $x non conforme
          exit 6
      fi
    else
      echo $x non numerico
      exit 7
    fi
    echo La $x linea del file $fname e:
    head -$x $i | tail -1
done

rm /tmp/conta$$



exit 0
