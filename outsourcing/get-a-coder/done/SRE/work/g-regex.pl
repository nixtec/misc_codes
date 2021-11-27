#!/usr/bin/env perl

use strict;
use warnings;
use GraphViz::Regex;

#my $regex = '(([abcd0-9])|(foo))';
my $regex = '^(a*b)*cd(e*f)*';

my $graph = GraphViz::Regex->new($regex);
print $graph->as_png;

