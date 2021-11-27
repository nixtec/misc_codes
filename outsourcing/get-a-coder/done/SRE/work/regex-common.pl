#!/usr/bin/env perl

use strict;
use warnings;
use Regexp::Common;

while (<>) {
  /$RE{num}{real}/			and print q{a number} . "\n";
  /$RE{delimited}{-delim=>'\/'}/	and print q{a /.../ sequence} . "\n";
  /$RE{balanced}{-parens=>'()'}/	and print q{balanced parentheses} . "\n";
}

