#!/usr/bin/env perl

use strict;
use warnings;

use Text::Balanced qw(
extract_delimited
extract_bracketed
extract_quotelike
extract_codeblock
extract_variable
extract_variable
extract_tagged
extract_multiple

gen_delimited_pat
gen_extract_tagged
);

my $text = 'ayub (a*b)*{x} ali';

my @result = extract_bracketed($text, '{}');

print join(":", @result) . "\n";
