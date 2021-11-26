#!/bin/sh

# view-progress.sh
# view-progress.sh <filename-you-specified-as-input>

if [ $# -eq 0 ] ; then
  echo "Usage: $0 <filename-you-specified-as-input-to-protocol6>"
  exit 1
fi

while [ 1 ] ; do
  ls -l $1 protocol6.out.server | cut -d ' '   -f 6
  sleep 1
done

exit $?
