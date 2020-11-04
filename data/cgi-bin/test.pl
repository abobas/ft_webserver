#!/usr/bin/perl



print "<h1>Environment</h1><hr>";
foreach (sort keys %ENV)
{
   print "<b>$_</b>: $ENV{$_}<br>\n";
}

print "<h2>STDIN</h2><hr>";
read (STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
print "$buffer <br>";