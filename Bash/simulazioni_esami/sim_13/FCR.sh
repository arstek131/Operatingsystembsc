#!/bin/sh

#$1
#$2 stringa S
#$3 Numero N
#$4 file temporaneo
cd $1


for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done

#controlli del caso
flag=0
for fname in *; do
  if test -f $fname -a -r $fname; then
    case $fname in
    *.$2) flag=1
          break
          ;;
    *) ;;
    esac
  fi
done

if test $flag -eq 1; then
  echo `pwd` >> $4
fi



exit 0
