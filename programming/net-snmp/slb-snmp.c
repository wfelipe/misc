#include "slb-snmp.h"
#include <sys/time.h>

int main (int argc, char **argv)
{
	struct farm_config fc;
	int i, n_values;
	char *value;
	char *values[BUFFER_SIZE], *names[BUFFER_SIZE];

	struct timeval tim;

	if (DEBUG)
	{
		gettimeofday (&tim, NULL);
		printf ("0: %ld %ld\n", tim.tv_sec, tim.tv_usec);
	}

	if (argc <= 1)
		help (USAGE);

	init_farm_config (&fc);
	help (read_farm_config (&fc, argv[1]));

	if (argc == 2)
	{
		print_farm_config (&fc);
		return 0;
	}

	if (DEBUG)
		printf ("argc: %d, prefix: %s\n", argc, PREFIX);

	for (i = 2, n_values = 0; i < argc; i++)
	{
		if (argv[i][strlen (argv[i]) - 1] == '+')
		{
			argv[i][strlen (argv[i]) - 1] = '\0';
			help (append_value_farm_item (&fc, argv[i], argv[i + 1]));
			names[n_values] = argv[i++];
		}
		else
			names[n_values] = argv[i];

		value = get_farm_item_value (&fc, names[n_values]);
		if (value == NULL)
			help (ER_FI_NAME);
		values[n_values++] = value;
	}

	if (DEBUG)
		print_farm_config (&fc);

	if (DEBUG)
	{
		gettimeofday (&tim, NULL);
		printf ("1: %ld %ld\n", tim.tv_sec, tim.tv_usec);
	}
	snmp_get_oids (&fc, names, values, n_values);
	if (DEBUG)
	{
		gettimeofday (&tim, NULL);
		printf ("2: %ld %ld\n", tim.tv_sec, tim.tv_usec);
	}

	help (OK);

	return 0;
}

/*
	function: handles the error codes, called by other functions
	input:
		err: error code
	output:
		return or prints the error and usage

*/
void help (int err)
{
	switch (err)
	{
		case OK:
			return;
		case USAGE:
			break;
		case ER_FILE_CNR:
			fprintf (stderr, "Configuration could not be found\n");
			break;
		case ER_SYNTAX:
			fprintf (stderr, "Syntax error\n");
			break;
		case ER_NOT_FOUND:
			fprintf (stderr, "farm_item not found\n");
			break;
		case ER_FI_NAME:
			fprintf (stderr, "farm_item name not found\n");
			break;
		default:
			fprintf (stderr, "unknown error\n");
			break;
	}

	printf ("usage: slb-snmp farm [config_name[+ append] [config_name[+ append] ...]]\n");
	printf ("\tfarm: farm config, localizado em %s/farm.conf\n", PREFIX);
	printf ("\t\tsem nenhum config_name, imprime a configuracao\n");
	printf ("\tconfig_name: faz a consulta com a mib especificada\n");
	printf ("\t\t+ append: adiciona o append na mib especificada\n");

	printf ("exemplos:\n");
	printf ("\tslb-snmp wwwfolha\n");
	printf ("\t\timprime a configuracao da wwwfolha\n");
	printf ("\tslb-snmp wwwfolha farm_conns host_conns+ 10.6.16.4.0\n");
	printf ("\t\tmostra as variaveis de farm_conns e host_conns.10.6.16.4.0\n");

	exit (1);
}

/*
	function: reads the configuration file, given by:
		PREFIX/config/_farm_.conf
	input:
		fc: struct that will keep the farm configuration
		farm: farm name
	output:
		OK: if everything goes ok, returns 1
		ER_FILE_CNR: if the configuration file, could not be read
		ER_SYNTAX: syntax error
*/
int read_farm_config (struct farm_config *fc, const char *farm)
{
	FILE *file;
	char *config_file;
	char config[BUFFER_SIZE], config_value[BUFFER_SIZE];

	config_file = fmalloc (	sizeof (char)*
		(strlen (PREFIX) + strlen ("/config/") + strlen (farm) + strlen (".conf")) + 1);
//	if (!config_file)

	config_file[0] = '\0';
	config_file = strcat (config_file, PREFIX);
	config_file = strcat (config_file, "/config/");
	config_file = strcat (config_file, farm);
	config_file = strcat (config_file, ".conf");

	if (DEBUG)
		printf ("config file: %s\n", config_file);

	file = fopen (config_file, "r");

	if (!file)
		return ER_FILE_CNR;

	while (fscanf (file, "%s", config) != EOF)
	{
		if (fscanf (file, "%s", config_value) == EOF)
			return ER_SYNTAX;
		add_farm_item (fc, config, config_value);
	}

	free (config_file);
	return OK;
}

/*
	function: prints farm_config members
	input:
		fc: struct to farm_config
*/
void print_farm_config (struct farm_config *fc)
{
	struct farm_item *fi;

	if (fc == NULL)
		return;

	if (DEBUG)
		printf ("******** print_farm_config ********\n");

	printf ("snmp_host:\t%s\n",		fc->snmp_host);
	printf ("snmp_community:\t%s\n",	fc->snmp_community);

	for (fi = fc->first; fi != NULL; fi = fi->next)
	{
		printf ("%s:\t", fi->name);
		printf ("%s\n", fi->oid);
	}
}

/*
	function: appends farm_config->fi{name} + . + value
	input:
		fc: farm_config
		name: configuration name
		value: value to append to the configuration name
	output:
		OK: everything went ok
		ER_FI_NAME: something wrong with the name or value
*/
int append_value_farm_item (struct farm_config *fc, char *name, char *value)
{
	struct farm_item *fi;

	if (name == NULL || value == NULL)
		return ER_FI_NAME;

	if ((fi = find_farm_item (fc, name)) == NULL)
		return ER_FI_NAME;

	strcat (fi->oid, ".");
	strcat (fi->oid, value);

	return OK;
}

void add_farm_item (struct farm_config *fc, char *name, char *value)
{
	struct farm_item *fi;

	if (strcmp (name, "snmp_host") == 0)
	{
		strcpy (fc->snmp_host, value);

		if (DEBUG)
			printf ("add_farm_item-> snmp_host: %s\n", fc->snmp_host);
		return;
	}
	if (strcmp (name, "snmp_community") == 0)
	{
		strcpy (fc->snmp_community, value);

		if (DEBUG)
			printf ("add_farm_item-> snmp_community: %s\n", fc->snmp_community);
		return;
	}

	if (fc->first == NULL)
	{
		fc->first = fmalloc (sizeof (struct farm_item));

		fc->first->next = NULL;
		fc->last = fc->first;

		strcpy (fc->first->name, name);
		strcpy (fc->first->oid, value);

		if (DEBUG)
			printf ("add_farm_item-> first item %s: %s\n", fc->first->name, fc->first->oid);
	}
	else
	{
		/* checks if the configuration hasn't been set already
			and replaces it (keeps the last one) */

		if ((fi = find_farm_item (fc, name)) != NULL)
		{
			strcpy (fi->name, name);
			strcpy (fi->oid, value);

			if (DEBUG)
				printf ("add_farm_item-> replacing %s: %s\n", fi->name, fi->oid);

			return;
		}

		/* new entry */
		fc->last->next = fmalloc (sizeof (struct farm_item));
		fc->last->next->next = NULL;
		fc->last = fc->last->next;

		strcpy (fc->last->name, name);
		strcpy (fc->last->oid, value);

		if (DEBUG)
			printf ("add_farm_item-> new item %s: %s\n", fc->last->name, fc->last->oid);
	}
}

struct farm_item *find_farm_item (struct farm_config *fc, char *name)
{
	struct farm_item *fi;

	if (fc->first == NULL)
		return NULL;

	fi = fc->first;

	do
	{
		if (strcmp (fi->name, name) == 0)
			return fi;
	} while ( (fi = fi->next) != NULL);

	return NULL;
}

void init_farm_config (struct farm_config *fc)
{
	fc->snmp_host[0]	= '\0';
	fc->snmp_community[0]	= '\0';

	fc->first		= NULL;
	fc->last		= NULL;
}

char *get_farm_item_value (struct farm_config *fc, const char *name)
{
	struct farm_item *fi = fc->first;

	while (fi != NULL)
	{
		if (strcmp (fi->name, name) == 0)
			return fi->oid;
		fi = fi->next;
	}

	return NULL;
}

void snmp_get_oids (struct farm_config *fc, char **names, char **values, int n_values)
{
	struct snmp_session session;
	struct snmp_session *sess_handle;
	struct snmp_pdu *pdu;
	struct snmp_pdu *response;
	struct variable_list *vars;
	oid id_oid[BUFFER_SIZE][MAX_OID_LEN];
	size_t id_len[BUFFER_SIZE];

	int status, i;

	if (DEBUG)
	{
		printf ("******** snmp_get_oids ********\n");
		printf ("snmp_host: %s\n", fc->snmp_host);
		printf ("snmp_community: %s\n", fc->snmp_community);
	}

	init_snmp ("slb-snmp");

	snmp_sess_init (&session);
	session.peername	= fc->snmp_host;
	session.community	= fc->snmp_community;
	session.community_len	= strlen (session.community);
	session.version		= SNMP_VERSION_1;

	sess_handle = snmp_open (&session);

	pdu = snmp_pdu_create (SNMP_MSG_GET);

	for (i = 0; i < n_values; i++)
	{
		id_len[i] = MAX_OID_LEN;
		read_objid (values[i], id_oid[i], &id_len[i]);
		snmp_add_null_var (pdu, id_oid[i], id_len[i]);
	}

	status = snmp_synch_response (sess_handle, pdu, &response);

	if (status != STAT_SUCCESS || response->errstat != SNMP_ERR_NOERROR)
		fprintf (stderr, "Error on executing snmp_synch_response\n");

	for (vars = response->variables, i = 0; vars; vars = vars->next_variable, i++)
	{
		switch (vars->type)
		{
			case ASN_INTEGER:
			case ASN_GAUGE:
				if (DEBUG) printf ("ASN_GAUGE\n");
				printf ("%s:%lld ", names[i], vars->val.integer[0]);
				break;
			case ASN_COUNTER:
				if (DEBUG) printf ("COUNTER32\n");
				printf ("%s:%lld ", names[i], vars->val.counter64[0].high);
				break;
			case ASN_OCTET_STR:
				if (DEBUG) printf ("STRING\n");
				printf ("%s:%s ", names[i], vars->val.string);
				break;

/*
			case ASN_NULL:
				fprintf (stderr, "OID not found, name %s, oid ", names[i]);
				fprintf (stderr, "%d", vars->name[0]);
				for (i = 1; i < vars->name_length; i++)
					fprintf (stderr, ".%d", vars->name[i]);
				fprintf (stderr, "\n");
*/
			default:
				fprintf (stderr, "OOPS, name: %s, value: %s\n", names[i], values[i]);
		}
	}
}

void *fmalloc (int size)
{
	void *ptr;

	ptr = malloc (size);

	if (!ptr)
	{
		printf ("Could not alloc memory\n");
		exit (1);
	}

	return ptr;
}
