#quando la radice NON va esplorata
for fname in $1/*; do
  if test -d $fname -a -x $fname; then
    testr.sh $fname $2 A
  fi
done

#la sua parta ricorsiva
cd "$1"
# recursion
for fname in *; do
	if [ -d "$fname" -a -x "$fname" ]; then
		$0 $(pwd)/"$fname" "$2" "$3"
	fi
done

#controlli del caso
#qua lavoro normalmente
