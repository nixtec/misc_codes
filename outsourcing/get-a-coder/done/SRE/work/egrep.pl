#!/usr/bin/env perl

use strict;
use warnings;

use Getopt::Std;

our($opt_v, $opt_e);
$opt_v = 0;
$opt_e = "";
#my $opt_v = 0;
my $inverse = 0;
my $regex = "";
getopts('ve:') or die("Command line parsing error\n");
if ($opt_v == 1) {
  $inverse = 1;
}

if ($opt_e ne "") {
  $regex = $opt_e;
} else {
  $regex = shift @ARGV;
}

my $nargs = $#ARGV + 1;
if ($regex eq "" and $nargs < 1) {
  print "Usage: $0 <regex> [FILE ...]\n";
  exit(1);
}

my $str = "";
my $filename = "/dev/tty";
if ($nargs >= 1) {
  while ($filename = shift @ARGV) {
    open(FH, $filename) || die("Can't open file '$filename': $!");
    while (defined($str = <FH>)) {
      chomp($str);
      if ($str =~ qr/$regex/is) {
	if ($inverse == 0) {
	  print "$str\n";
	}
      } else {
	if ($inverse == 1) {
	  print "$str\n";
	}
      }
    }
    close(FH);
  }
} else {
  while (defined($str = <STDIN>)) {
    chomp($str);
    if ($str =~ qr/$regex/isx) {
      if ($inverse == 0) {
	print "$str\n";
      }
    } else {
      if ($inverse == 1) {
	print "$str\n";
      }
    }
  }
}
