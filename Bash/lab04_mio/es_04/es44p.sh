#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 3; then
  echo Numero di parametri sbagliato
  exit 1
fi

#mi salvo in una variabile temporanea il primo parametro
tmp=$1
shift

#controllo ora che si tratti di un nome relativo semplice di
#file F

case $tmp in
  /*|*/*) echo Plz solo nomi relativi semplici
          exit 2
          ;; #escludo i casi non relativi semplici
esac

#controllo ora che $tmp sia un file F

if test ! -f $tmp; then
  echo Non si tratta di un file
  exit 3
fi

#controllo ora i $* parametri assicurandomi che siano nomi ass di
#direttori
for i
do
  case $i in
    /*) ;;
    *) echo Soli nomi assoluti plz
       exit 4
       ;;
  esac

  if test ! -d $i -o ! -x $i ; then
    echo Solo direttori plz
    exit 5
  fi
done

export PATH=$(pwd):$PATH
#Faccio la chiamata ricorsiva ciclica perchè devo controllare
#Ognuna delle Gi gerarchie
totalFiles=0
for i
do
  > /tmp/perOgni #creo il file temporaneo che conterra
                  #il numero di file creato per ogni Gi
  es44r.sh $i $tmp /tmp/perOgni
  linee=`wc -l < /tmp/perOgni`
  totalFiles=$(expr $totalFiles + $linee)
  echo Il numero di file creato per $i e = $linee
  rm -rf /tmp/perOgni >/dev/null 2>&1
done

echo Il numero globale di file creato e $totalFiles

exit 0















