#!/bin/sh

cd $1

for i in *
do
    if test -f $i
    then echo `pwd`/$i >> $2
    fi
done

#parte ricorsiva

echo in `pwd` conta = `wc -l` < $2

for i in *
do
    if test -d $1 -a -x $1
    then $0 $1 $2
    fi
done





