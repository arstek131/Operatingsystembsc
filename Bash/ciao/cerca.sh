#!/bin/sh

#file comandi cerca.sh
#ricerca in breadth-first

cd $1

if [ -f $2 ]; then
    echo il file $2 si trova in `pwd`
fi

for i in *
do
if [ -d $i -a -x $i ]; then
    echo Stiamo per esplorare il direttorio `pwd`/$i
    cerca.sh `pwd`/$i $2
fi
done

