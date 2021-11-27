#!/usr/bin/env perl

# Copyright (C) Ayub <mrayub@gmail.com>

# Program name: $0
# Argument List: @ARGV
# function/subroutine argument list: @_
# default 'feature' doesn't allow using 'break', use 'last'

#use strict;
use warnings;

# use of global is needed because of limitations of (?{ code }) implementation
# normally (?{code}) is a compile-time feature. the code is not evaluated
# every time. a workaround is to use global variable inside 'code'
#
# global variables
#
# Gagan:
# g_vars array contains the list of exponents specified in the regex
my @g_vars = (); # this array contains the variables (x, y, z, etc...)

# hash is used so that duplicate variables are not included more than once
# Gagan:
# g_hash (is a hash) is used to handle the multiple reference of the same
# exponent in the regex. For example, if you use {x} in more than one places
# we won't add it to g_vars most than once. Using array this checking is very
# inefficient
my %g_hash = ();

# Gagan:
# g_regex will contain the final regex that we will build for evaluation
# in matching, by replacing every {x} instance with {$x}. So that when we will
# set value of $x in loops for testing, it will be substituted with its value
my $g_regex = '';
my $var = '';
my $debug = 0;

my $nargs = $#ARGV + 1;

if ($nargs < 1) {
  print "Usage: $0 <regex> [str]\n";
  exit(1);
}

$g_regex = $ARGV[0];
# (?{code}): see 'man perlre' to understand this command
# code can be any perl code to be executed upon matching
# Gagan:
# See that '!exists()' is used. So if the exponent we added to @g_vars earlier
# we are not going to add it (this is ensured by using a hash (%g_hash)
$g_regex =~ s/([^\\{]?){([a-zA-Z]+)(?{ $var = $^N; if (!exists($g_hash{$var})) { $g_hash{$var} = 0; push(@g_vars, $var);} })}/$1\{\$$2}/g;
if ($debug == 1) {
  print "regex=$g_regex\n";
  print "vars = @g_vars\n";
}


my $str = "";
if ($nargs > 1) {
  $str = $ARGV[1];
  do_match_count($str);
} else {
  print "[ INPUT ] << ";
  while (defined($str = <STDIN>)) {
    chomp($str);
    do_match_count($str);
    print "[ INPUT ] << ";
  }
  print "\n";
}

# here goes the function
sub do_match_count {
  my $str1 = shift(@_);
  my $maxlen = length($str1);

  my $regex = $g_regex;
  my @vars = @g_vars;
  my %hash = %g_hash;

  my $pre_eval = '';
  for $v (@vars) {
    $pre_eval = "\$$v = 0;"; # Initialize the exponents to 0
    eval($pre_eval);
  }

  my $v = '';
  my $key = '';
  my $evalstr = '';
  my @append = ("\n");
  my $matched = 0;
  my $nvars = @vars;
  if ($nvars > 0) {
    for $v (@vars) {
      push(@append, "\t"); # to make the code look better (in debug mode)
      # Gagan:
      # We are building the code that we need to execute for testing matching
      # by putting values in the exponents specified by user
      $evalstr .= "for (\$$v = 0; \$$v <= $maxlen; \$$v++) {" . join('', @append);
    }
    pop(@append); # to make the code output look better (for debugging)
    $evalstr .= "if ('$str1' =~ /$regex/) { for \$key (keys \%hash) { \$matched = 1; \$hash{\$key} = \$\$key; } }" . join('', @append);
    for $v (@vars) {
      pop(@append);
      $evalstr .= "}" . join('', @append);
    }
    if ($debug == 1) {
      print "Dynamic code to execute:\n";
      print "========================\n";
      print "$evalstr\n";
      print "========================\n";
    }
  } else {
    $evalstr = "if ('$str1' =~ /$regex/) { \$matched = 1; } ";
  }

  # dynamic regex and code creation done here, now evaluate it
  eval $evalstr;

  my $post_eval = '';
  my $first = 1;
  if ($matched == 1) {
    print "[MATCHED] >> ";
  } else {
    print "[NOMATCH] >> ";
  }
  print "{";
  for $key (keys %hash) {
    if ($first == 0) {
      print ", ";
    } else {
      $first = 0;
    }
    print "$key = $hash{$key}";
  }
  print "} => $str1";
  print "\n";

  return 0;
}

