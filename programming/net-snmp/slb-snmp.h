#ifndef __SLB_SNMP_H__
#define __SLB_SNMP_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define BUFFER_SIZE	255

/* ERRORS */
#define ER_FILE_CNR	-1 /* could not read */
#define ER_SYNTAX	-2 /* syntax error */
#define ER_NOT_FOUND	-3 /* configuration file not found */
#define ER_FI_NAME	-4 /* error name in farm_item */

#define OK		1
#define USAGE		2

struct farm_item
{
	struct farm_item *next;

	char name[BUFFER_SIZE];
	char oid[BUFFER_SIZE];
} farm_item;

struct farm_config
{
	char farm_name[BUFFER_SIZE];
	char snmp_host[BUFFER_SIZE];
	char snmp_community[BUFFER_SIZE];

	struct farm_item *first;
	struct farm_item *last;
} farm_config;

void *fmalloc (int size);

void help (int err, char *cerr);

int read_farm_config (struct farm_config *fc, const char *farm);

void print_farm_config (struct farm_config *fc);
void init_farm_config (struct farm_config *fc);

void add_farm_item (struct farm_config *fc, char *name, char *oid);
struct farm_item *find_farm_item (struct farm_config *fc, char *name);
int append_value_farm_item (struct farm_config *fc, char *name, char *value);

char *get_farm_item_value (struct farm_config *fc, const char *name);

/*
	SNMP
*/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

void snmp_get_oids (struct farm_config *fc, char **names, char **values, int n_values);

#endif
