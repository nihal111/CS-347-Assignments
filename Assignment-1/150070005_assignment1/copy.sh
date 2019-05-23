#!/bin/bash
start=`date +%s`

if [ -z "$1" ]
  then
    echo "Missing first parameter- file name"
    exit
fi

FILENAME=$1

if [ ! -f $FILENAME ]; then
    echo "Specified file -\"$FILENAME\" not found."
fi

if [ -z "$2" ]
  then
    echo "Missing second parameter- number of copies"
   exit
fi

N=$2

if ! ([[ $N =~ ^[\-0-9]+$ ]] && (( N > 0))); then
  echo "$N is not a positive number. Please enter a valid second parameter."
  exit
fi

mkdir files
for ((i = 1; i <= N; i++)); do
    cp $FILENAME files/$FILENAME$i
done

end=`date +%s`

runtime=$((end-start))
echo $runtime
