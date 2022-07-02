#!/bin/sh

#controllo i parametri

if test $# -le 2; then
  echo Numero di parametri sbagliato
  exit 1
fi

#faccio i controlli sul primo parametro
case $1 in
  */*) echo Nome relativo semplice pls
       exit 2
       ;;
  *) ;;
esac


tmp=$1
shift

#controllo ora i $* parametri assicurandomi che siano nomi assoluti di direttori
for i
do
    case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non direttorio
	    exit 3
	    fi;;
	*)  echo $i non nome assoluto; exit 4;;
	esac
done

#ho finito il controllo sui parametri mi preparo a lanciare le N fasi

export PATH=$(pwd):$PATH

#creo il file temporaneo
>/tmp/ess45

#faccio la chiamata ricorsiva ciclica per ogni file

for i
do
  es45r.sh $i $tmp /tmp/ess45
done

linee_tot=`wc -l < /tmp/ess45`

echo In totale sono stati creati $linee_tot file

for i in `cat /tmp/ess45`
do
  case $i in
  *.noline) echo empty
            ;;
   *) cat <  $i
      ;;
  esac
done


rm -rf /tmp/ess45

exit 0


