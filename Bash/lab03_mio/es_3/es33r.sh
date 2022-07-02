#!/bin/sh

cd "$1"

#chiamata ricorsiva

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
      "$0" "$fname" "$2" "$3"
  fi
done

#eseguo i controlli del caso

for fname in *; do
  if [ ! -f "$fname" -o ! -r "$fname" ]; then
  continue #se non è un file e non è leggibile continua
fi

case "$fname" in #controllo che nel nome abbia entrambi i caratteri
    *$2*$3*) ;;  #se non è così continua
    *$3*$2*) ;;
    *) continue
     ;;
esac

#ora cerca nel contenuto di questi 2 file

grep -q $2 "$fname" #modalita quite, soppressa l'output
if [ $? -ne 0 ]; then
    continue
fi


grep -q $3 "$fname" #modalita quite, soppressa l'output
if [ $? -ne 0 ]; then
    continue
fi

echo $(pwd)

done

exit 0





