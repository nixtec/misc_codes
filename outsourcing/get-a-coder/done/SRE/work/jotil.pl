#!/usr/bin/env perl

#my @fields = split(/{([^}])}/, "ab{x}c{y}");
#print "@fields\n";

$_ = "ab{x}c{y}d{z}";
s/{([^}])(?{ $var = $^N; print "$var\n"; })}/{\$\1}/g;
#print "var = $var\n";

#$_ = "The brown fox jumps over the lazy dog";
#/the (\S+)(?{ $color = $^N }) (\S+)(?{ $animal = $^N })/i;
#print "color = $color, animal = $animal\n";
