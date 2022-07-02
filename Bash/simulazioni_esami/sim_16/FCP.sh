#!/bin/sh

#controllo il numero dei parametri
if test $# -le 2; then
  echo Numero paremetri sbagliato
  exit 1
fi

#controllo che si tratti di una directory e che sia assoluta
tmp=1
X=0
for i
do
if test $# -eq $tmp; then
  X=$i
  break
fi
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non directory
	    exit 2
	    fi;;
	*)  echo $i non nome assoluto; exit 3;;
	esac
    tmp=`expr $tmp + 1`
done

#controllo che si tratti di un numero e che sia >0
expr $X + 0  > /dev/null 2>&1
if test $? -ne 2 -a $? -ne 3
  then
     if test $X -le 0
       then echo $X non positivo
          exit 4
     fi
else
  echo $X non numerico
  exit 5
fi

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

> /tmp/conto$$

num_par=1

for i
do
  if test $num_par -eq $#; then
    break
  fi
  FCR.sh $i $X /tmp/conto$$
  num_par=`expr $num_par + 1`
done

tot=`wc -l < /tmp/conto$$`
echo In totale sono stati trovati $tot direttori

echo Passo alla SECONDA FASE

for i in `cat /tmp/conto$$`
do
  echo Trovato il direttorio $i
  cd $i
  for j in *; do
    echo IL FILE $j
    echo La sua linea $X dalla fine
    tail -$X $j | head -1
  done
done

rm /tmp/conto$$


exit 0
