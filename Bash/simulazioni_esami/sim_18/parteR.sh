#!/bin/sh

cd $1

for fname in *; do
  if test -d $fname -a -x $fname; then
    $0 $fname $2 $3
  fi
done


for i in *
do
    #controllo: se è un file ed è leggibile fai i controlli del caso
    #altrimenti continua
    if test -f $i -a -r $i
    then
      #faccio i controlli del caso
      numr_lin=`wc -l < $i` #ho il numero delle linee del file $i
      if test $numr_lin -ge $2
      then
          if test $numr_lin -ge 5
          then
          #se il file ha >=5 linee
          #caso f.quinta
          head -5 $i | tail -1 > $i.quinta
          echo $i.quinta >> $3
          else
          #caso in cui il file non abbia almeno 5 righe
          #quindi caso f.NOquinta
          > $i.NOquinta
          echo $i.NOquinta >> $3
          fi
      fi
    else
      continue
    fi
done
