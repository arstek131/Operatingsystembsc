#!/bin/sh

# $1 contiene il direttorio corrente
# $2 contiene il nome di direttorio semplice
# $3 contiene le fasi (A e B)

cd "$1"

#chiamata ricorsiva

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
    $0 "$fname" "$2" "$3"
  fi
done

if [ "$3" == "A" ]; then
  if [ -d "$2" ]; then
    echo $(pwd)
  fi
fi


if [ "$3" == "B" ]; then
  if [ ! -d "$2" ]; then
    mkdir "$2"
  fi
fi



exit 0
