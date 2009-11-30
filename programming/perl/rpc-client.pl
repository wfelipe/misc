#!/usr/bin/perl
#
#
#
use strict;
use RPC::XML;
use RPC::XML::Client;
use Data::Dumper;

my $client = RPC::XML::Client->new ('http://localhost:9000/');
my $response;

$response = $client->send_request ('method', {
	host => 'host',
	port => 'port',
});

print Dumper ($response);
