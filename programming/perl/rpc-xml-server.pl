#!/usr/bin/perl
#
#
#
use strict;
use warnings;

use RPC::XML::Server;

my $srv = RPC::XML::Server->new (port => 9000);

$srv->add_method ({	name => 'teste.wfelipe',
			signature => [ 'int' ],
			code => sub { return 1; } });

$srv->server_loop;
