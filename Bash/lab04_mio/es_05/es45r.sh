#!/bin/sh

cd $1

#chiamata ricorsiva
for file in *; do
  if test -d $file -a -x $file ; then
    $0 $file $2 $3
  fi
done



#controlli del caso

#se il nome relativo non coincide, esco
if test $(basename $(pwd)) != $2; then
  exit 0
fi

#caso in cui il nome coincide
for fname in *; do
  #se è un file ed è leggibile
  if test -f $fname -a -r $fname; then
    totline=`wc -l < $fname`
    if test $totline -lt 3; then #se il file ha meno di 3 linee
      >$fname.noline
      echo $($pwd)/$fname.noline >> $3
    else
      ter_ultima=`tail -3 $fname | head -1`
      >$fname.line
      echo $ter_ultima >> $fname.line
      echo $($pwd)/$fname.line >> $3
    fi
  fi
done

exit 0


