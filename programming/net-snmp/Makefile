CC			= gcc -D'DEBUG=0'
CFLAGS			= -Wall -O2 -march=i586 -mcpu=i686
LIBS			= -lnetsnmp -lcrypto
OBJS			=
PREFIX			= /opt/UOLI/slb-snmp

#SNMP_LIBS		= net-snmp-config --libs
#SNMP_EXTERNAL_LIBS	= net-snmp-config --external-libs
#SNMP_CFLAGS		= net-snmp-config --cflags

all: slb-snmp

slb-snmp: slb-snmp.c
	$(CC) -D'PREFIX="$(PREFIX)"' $(CFLAGS) $(LIBS) $(OBJS) $@.c -o $@

clean:
	rm *.o slb-snmp a test
