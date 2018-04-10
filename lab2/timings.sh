#!/bin/sh

if [ -z $1 ]
then
  echo "Need arguments: <exec>"
  exit 1
fi

TEST_EXEC=$1

for i in 1 2 3 4
do
  echo "==== Processes: $i ===="
  ( { time -p $TEST_EXEC $i > /dev/null; } 2>&1 )
done

