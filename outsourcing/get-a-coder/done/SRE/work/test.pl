#!/usr/bin/env perl

#my @fields = split(/{([^}])}/, "ab{x}c{y}");
#print "@fields\n";

my $var = "";
my @a = ();
my $regex = "ab{x}c{y}d{z}";
$regex =~ s/{([^}])(?{ $var = $^N; push(@a, $var);})}/{\$\1}/g;
print "$regex\n";
print "@a\n";

#print "var = $var\n";

#$_ = "The brown fox jumps over the lazy dog";
#/the (\S+)(?{ $color = $^N }) (\S+)(?{ $animal = $^N })/i;
#print "color = $color, animal = $animal\n";