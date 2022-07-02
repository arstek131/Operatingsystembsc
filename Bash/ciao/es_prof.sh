#!/bin/sh


case $# in
    1) case $1 in
              /*) if test ! -d $1 -o ! -x $1
     then
              echo $1 non direttorio o non traversabile
              exit 1
     fi;;
     *) echo $1 non nome assoluto; exit 2;;
     esac;;
*) echo Errore
       exit 3;;
esac

PATH=`pwd`:$PATH

export PATH

>/tmp/conta$$

contaB.sh $1 /tmp/conta$$

echo il numero di file totale della gerarchia = wc -l < /tmp/conta$$





