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
my @g_vars = ();
my %g_hash = ();
my $g_regex = '';
my $var = '';


my $debug = 1;
sub do_match_count {
  my $str = shift(@_);
  my $maxlen = length($str);

  my $regex = $g_regex;
  my @vars = @g_vars;
  my %hash = %g_hash;

  my $evalstr = '';
  my $pre_eval = '';
  for $v (@vars) {
    $pre_eval = "\$$v = 0;";
    eval($pre_eval);
  }

  my $count = 0;
  my $v = '';
  for $v (@vars) {
    $evalstr = '';
#    $evalstr .= "print \"$v = \$$v\";\n";
    $evalstr .= "\$count = 0;\nfor (\$$v = 0; \$$v <= $maxlen; \$$v++) ";
#    $evalstr .= "{\n\tif (\"$str\" !~ /$regex/) {\n\t\tlast;\n\t}\n}\n";
    $evalstr .= "{\n\tif ('$str' =~ /$regex/) {\n\t\tprint(\"matched at \$$v\"); \$count = \$$v;\n\t}\n}\n";
    $evalstr .= "\$$v = \$count;";
#    $evalstr .= "if (\$count > 0) { \$count -= 1; \$$v = \$count; }";
#    $evalstr .= "\nprint \">> str = \$str, $v = \$$v\";";
    if ($debug == 1) {
      print "Dynamic code to execute:\n";
      print "========================\n";
      print "$evalstr\n";
      print "========================\n";
    }
    print "EVALUATION|";
    eval $evalstr;
    print "|EVALUATION";
#    print "\n";
  }

#  @vars = @g_vars;
  my $post_eval = '';
  my $first = 1;
  for $v (@vars) {
    if ($first == 0) {
      $post_eval .= ", ";
    } else {
      $post_eval = "print \">> $str: ";
      $first = 0;
    }
    $post_eval .= "$v = \$$v";
#    print "pre_eval: $pre_eval\n";
  }
  $post_eval .= "\";";
  if ($debug == 1) {
    print "Dynamic code to execute:\n";
    print "========================\n";
    print "$post_eval\n";
    print "========================\n";
  }
  eval($post_eval);
  print "\n";

  return 0;
}

my $nargs = $#ARGV + 1;

if ($nargs < 1) {
  print "Usage: $0 <regex> [str]\n";
  exit(1);
}

$g_regex = $ARGV[0];
#$g_regex =~ s/\\\$/\\\$/g;
$g_regex =~ s/([^\\{]?){([a-zA-Z]+)(?{ $var = $^N; if (!exists($g_hash{$var})) { $g_hash{$var} = 0; push(@g_vars, $var);} })}/$1\{\$$2}/g;
print "regex=$g_regex\n";
print "vars = @g_vars\n";

#exit(0);

my $str = "";
if ($nargs > 2) {
  $str = $ARGV[1];
  $str =~ s/\$/\\\$/g;
  do_match_count($str);
} else {
  while (defined($str = <STDIN>)) {
    chomp($str);
    $str =~ s/\$/\\\$/g;
    do_match_count($str);
  }
}

