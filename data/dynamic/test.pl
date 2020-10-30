#!/usr/bin/perl

foreach my $key (keys %ENV) {
    print "$key --> $ENV{$key}<br>";
}