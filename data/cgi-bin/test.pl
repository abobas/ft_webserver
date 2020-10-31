#!/usr/bin/perl
print " <title> CGI script </title> ";
read (STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
foreach my $key (keys %ENV) {
    print "$key --> $ENV{$key}<br>";
}
print "$buffer <br>";