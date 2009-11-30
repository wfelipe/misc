#!/usr/bin/perl
#
#
#
use strict;
use warnings;

use RPC::XML::Server;
use Data::Dumper;

my $srv = RPC::XML::Server->new (port => 9000);

$srv->add_method ({	name => 'teste.wfelipe',
	signature => [ 'string struct' ],
	code => \&method
});

$srv->server_loop (
	server_type => qw( Fork Single ),
	max_servers => 16,
);

sub method {
	my %args = %{ shift () };

	return "string";
}
