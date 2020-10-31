#!/usr/bin/perl
print " <title> CGI script </title> ";
foreach my $key (keys %ENV) {
    print "$key --> $ENV{$key}<br>";
}