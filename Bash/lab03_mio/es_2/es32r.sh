#!/bin/sh

# $1 contiene il direttorio attuale
# $2 contiene il nome del file

cd "$1"

#chiamata ricorsiva
for filename in *; do
  if [ -d "$filename" -a -x "$fname" ]; then
      #chiamata ricorsiva
      "$0" "$filename" "$2"
  fi
done

#eseguo i controlli del caso

#controllo se esistono i file F1 e F2
if [ ! -f "$2".1 -o ! -r "$2".1 ]; then
  exit 4 #se non si tratta di un file che si chiama .1 oppure
fi       #oppure questo file non è leggibile esci

if [ ! -f "$2".2 -o ! -r "$2.2" ]; then
  exit 4 #stesso controllo di prima
fi

#controllo ora la lunghezza in linee

#mi salvo in due variabili man mano num righe e caratteri

tmp1l=$(wc -l < "$2".1)
tmp1c=$(wc -c < "$2".1)

tmp2l=$(wc -l < "$2".2)
tmp2c=$(wc -c < "$2".2)

#se il numero di linee e caratteri coincidono, stampo

if [ "$tmp1l" -eq "$tmp2l" -a "$tmp1c" -eq "$tmp2c" ]; then
    echo $(pwd)
fi

exit 0





