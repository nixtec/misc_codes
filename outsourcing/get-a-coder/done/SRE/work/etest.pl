#!/usr/bin/env perl
#

$pattern = "((a*b)*){2}";

$re = qr/$pattern/x;

$string = "aaaabab";

if ($string =~ $re) {
  print "Matched\n";
  print ${^MATCH};
} else {
  print "Not matched\n";
}

