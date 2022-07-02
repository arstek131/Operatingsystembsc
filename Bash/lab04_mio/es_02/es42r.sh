#!/bin/sh

TMPFILE=/tmp/flist
SUMFILE=SOMMA

cd "$1"

#chiamata ricorsiva

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
    "$0" "$fname" "$2" "$3"
  fi
done

#controlli del caso

if [ $(basename) $(pwd))) != "$2" ]; then #basename estrae il nome del file da un percorso
 exit 0
fi

rm -rf "$SUMFILE" >/dev/null 2>&1

contatore=0 #per evitare di ristampare continuamente il nome del direttorio che contiene il file F, faccio il controllo con un contatore per vedere se in quella directory c'è un file che mi ha soddisfatto la condizione
for fname in *; do
  if [ -f "$fname" ]; then
    if [ $(wc -c < "$fname") -gt "$3" ]; then
      if [ "$contatore" -eq 0 ]; then
          echo $(pwd)
      fi
      contatore=$(expr "$contatore" + 1)

      echo $(pwd)/"$fname" >> $TMPFILE
      cat "$file" >> "$SUMFILE"
    fi
  fi
done

exit 0









