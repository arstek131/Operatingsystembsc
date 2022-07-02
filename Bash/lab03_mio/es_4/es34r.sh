#!/bin/sh

cd "$1"

shift #il vecchio $2 è diventato $1

#chiamata ricorsiva

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
    $0 "$fname" "$*"
  fi
done

#controlli del caso

#controllo che si tratti di un file e che sia leggibile
for fname in *; do
  if [ ! -f "$fname" -o ! -r "$fname" ]; then
    continue
fi

#controllo la dimensione del file che sia pari
dim=$(wc -c < "$fname")
resto=`expr $dim % 2`
if [ $resto -ne 0 ]; then
  continue
fi

#controllo il contenuto

missing=N
for car in $*; do #per tutti i caratteri (per questo ho shiftato subito)
  grep $car "$fname" > /dev/null 2>&1
  if [ $? -ne 0 ]; then #se non hai trovato il carattere nel file, attiva
    missing=Y           #il flag
    break
  fi
done

if [ "$missing" == "Y" ]; then
  continue
fi

echo $(pwd)/"$fname"

done

exit 0






