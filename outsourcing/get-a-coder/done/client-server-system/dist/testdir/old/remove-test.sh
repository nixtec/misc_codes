#!/bin/sh

FILE="remtest.file"

cat > "$FILE" << EOF
01.the quick brown fox jumps over the lazy dog
02. the quick brown fox jumps over the lazy dog
03.  the quick brown fox jumps over the lazy dog
04.   the quick brown fox jumps over the lazy dog
05.    the quick brown fox jumps over the lazy dog
06.     the quick brown fox jumps over the lazy dog
07.      the quick brown fox jumps over the lazy dog
08.       the quick brown fox jumps over the lazy dog
09.      the quick brown fox jumps over the lazy dog
10.     the quick brown fox jumps over the lazy dog
11.    the quick brown fox jumps over the lazy dog
12.   the quick brown fox jumps over the lazy dog
13.  the quick brown fox jumps over the lazy dog
14. the quick brown fox jumps over the lazy dog
15.the quick brown fox jumps over the lazy dog
EOF

rm -f "remove-test"
gcc -g -Wall -o "remove-test" "remove-test.c"
if [ $? -eq 0 ] ; then
  ./"remove-test" "$FILE"
  rm -f "remove-test"
fi

