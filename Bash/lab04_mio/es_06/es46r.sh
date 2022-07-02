#!/bin/sh

#$2 numero M
#$3 file temporaneo

cd $1

#chiamata ricorsiva
for fname in *; do
  if test -d $fname -a -x $fname; then
    $0 $fname $2 $3
  fi
done



#controlli del caso

flag=0

for fname in *; do
  if test -f $fname -a -r $fname; then
    #controllo il numero delle linee
    tmp_linee=`wc -l < fname`
    if test $tmp_linee -ne $2; then
      continue
    else
      flag=1
    fi
  fi
done

if test $flag -eq 1; then
  for fname in *; do
    if test -f $fname -a -r $fname; then
      linea_temp=`wc -l < fname`
        if test $linea_temp -lt 3; then
          >$fname.noline
          echo $(pwd)/$fname.noline >> $3
        else
          ter_ultima=`tail -3 $fname | head -1`
          >$fname.line
          echo $ter_ultima >> $fname.line
          echo $($pwd)/$fname.line >> $3
        fi
    fi
  done
fi


exit 0





