#!/usr/bin/perl -W
#
# wfelipe <wpereira@uolinc.com>
#
# reads webserver access log (ncsa), and computes the
# benefit of having a givin keepalive timeout (default to 4)
#
# in the end, prints the results as is:
# - how many connections would be closed within the timeout
#
use strict;

my %ips = ();
my @results = ();
my $newConnection;
my $keepAliveTimeout = ($ARGV[0])? $ARGV[0]: 4;

print "keepAliveTimeout: $keepAliveTimeout\n";

my $lastSecond = 0;
my $second = 0;
while (my $line = <STDIN>)
{
	chomp ($line);
	#
	# $1: ip
	# $2: hour
	# $3: minute
	# $4: second
	#
	$line =~ /^(\S+)[^\[]+(\d+):(\d+):(\d+)$/;

	# minutes*60 + seconds
	$second = $3*60 + $4;
	$ips{$1}{'last'} = $second;

	if ($second != $lastSecond)
	{
		$lastSecond = $second;
		&checkConnections ($second);
	}
}

sub checkConnections
{
	my $second = shift;
	my $alive = 0;

	$newConnection = 0;
	foreach my $ip (keys %ips)
	{
		if ( $ips{$ip}{'last'} > ($second - $keepAliveTimeout) )
		{
			$newConnection++
				if (!$ips{$ip}{'alive'} || $ips{$ip}{'alive'} == 0);
			$ips{$ip}{'alive'} = 1;
			$alive++;
		}
		else
		{
			$ips{$ip}{'alive'} = 0;
		}
	}

	print "$second: $alive, newConn: $newConnection\n";
}

