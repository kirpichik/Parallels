#!/bin/sh

if [ -z $1 ]
then
  echo "Need arguments: <exec>"
  exit 1
fi

TEST_EXEC=$1

for i in 1 2 4 8 16
do
  echo "==== Processes: $i ===="
  ( { time -p mpirun -n $i $TEST_EXEC > /dev/null; } 2>&1 )
done

