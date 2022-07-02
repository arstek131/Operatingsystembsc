#!/bin/sh

#controllo il numero dei parametri
if test $# -le 1; then
  echo Numero paremetri sbagliato
  exit 1
fi

#controllo che si tratti di una directory e che sia assoluta
for i
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non directory
	    exit 2
	    fi;;
	*)  echo $i non nome assoluto; exit 3;;
	esac
done

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

n=1

for i
do
	echo fase per $i
    > /tmp/conta$$-$n
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR.sh $i /tmp/conta$$-$n
    n=`expr $n + 1`
done

#terminate le N fasi
n=1
X=0
for i
do
  echo Fase per la gerachia $i
  NL=`wc -l < /tmp/conta$$-$n`
  echo Ha $NL che soddisfano il criterio
  for j in `cat /tmp/conta$$-$n`
  do
    echo Trovato il file $j
    echo Inserisci il numero X
    read X
      #controllo che si tratti di un numero e che sia >0
      expr $X + 0  > /dev/null 2>&1
      if test $? -ne 2 -a $? -ne 3
      then
        if test $X -le 0
        then echo $1 non positivo
          exit 4
        fi
      else
        echo $X non numerico
        exit 5
      fi
      head -$X < $j
  done
  rm /tmp/conta$$-$n
  n=`expr $n + 1`
done



exit 0
