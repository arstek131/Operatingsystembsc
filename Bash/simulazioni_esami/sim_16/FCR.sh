#!/bin/sh

cd $1

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i $2 $3
	fi
done

#controlli del caso
flag1=0
flag2=0
NL=0
for fname in *; do
  if test -f $fname -a -r $fname; then
    NL=`wc -l < $fname`
    if test $NL -le $2; then
      flag1=1
    fi
  else
    if test -d $fname; then
    flag2=1
    fi
  fi
done

if test $flag1 -eq 0 -a $flag2 -eq 0 -a $NL -ne 0; then
 echo `pwd` >> $3
fi

exit 0
