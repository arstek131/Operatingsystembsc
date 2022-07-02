#!/bin/sh

# $1 contiene il direttorio
# $2 contiene la profondità attuale

cd "$1"

#chiamata ricorsiva

for fname in *; do
  if [ -d "$fname" -a -x "$fname" ]; then
    $0 `pwd`/"$fname" $(expr $2 + 1) #incremento la profondita in caso di success
  fi
done

echo "[$2] $(pwd)"
if [ $2 -gt $(cat /tmp/maxlevel) ]; then
  echo "$2" > /tmp/maxlevel
fi

exit 0
