#!/usr/bin/perl

use strict;
use warnings;

foreach my $key (keys %ENV) {
    print "$key --> $ENV{$key}<br>";
}