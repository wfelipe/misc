#!/usr/bin/perl -w
#
# wfelipe <wpereira@uolinc.com>
#
# contabiliza os bytes sent do log accesslog do apache,
# lendo do stdin
#
use strict;

my (%files, @line);
my ($method, $url, $code, $bsent, $referer);
my $debug = 1;

open (INPUT, "<&STDIN");

while (<INPUT>)
{
	chomp ($_);

	@line = split (/ +/, $_);

	($method, $url, $code, $bsent, $referer) = ($line[5], $line[6], $line[8], $line[9], $line[10]);

	next if ( ($method ne "\"GET") && ($code ne "200") );

	&debug if ($debug);
}

sub debug
{
	print "method: $method\n";
	print "url: $url\n";
	print "code: $code\n";
	print "bytes sent: $bsent\n";
	print "referer: $referer\n";

	my $i = 0;
	foreach (@line)
	{
		print "$i: $_\n";
		$i++;
	}
}
