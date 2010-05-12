#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/hmac.h>
#include <time.h>

static void c2c(uint64_t counter, unsigned char challenge[8]);
static void hotp(unsigned char challenge[], unsigned char keyblock[], unsigned char response[]);

static void hotp(unsigned char challenge[], unsigned char keyblock[], unsigned char response[])
{
	uint32_t dbc;		/* "dynamic binary code" from HOTP draft */
	unsigned char md[20];
	unsigned md_len;

	/* 1. hmac */
	(void)HMAC(EVP_sha1(), keyblock, 20, challenge, 8, md, &md_len);

	/* 2. the truncate step is unnecessarily complex */
	{
		int offset;

		offset = md[19] & 0x0F;
		/* we can't just cast md[offset] because of alignment and endianness */
		dbc = (md[offset] & 0x7F) << 24 |
		    md[offset + 1] << 16 | md[offset + 2] << 8 | md[offset + 3];
	}

	/* 3. int conversion and modulus (as string) */
	fprintf(stdout, "fullkey: %d\n", dbc);
	(void)sprintf((char *)response, "%06lu", dbc % 1000000L);
}

static void c2c(uint64_t counter, unsigned char challenge[8])
{
	challenge[0] = counter >> 56;
	challenge[1] = counter >> 48;
	challenge[2] = counter >> 40;
	challenge[3] = counter >> 32;
	challenge[4] = counter >> 24;
	challenge[5] = counter >> 16;
	challenge[6] = counter >> 8;
	challenge[7] = counter;
}

int main(int argc, char **argv)
{
	char *username = NULL;
	char *secret = NULL;
	char response[7];
	int debug = 0;
	int opt;
	int timestamp;
	unsigned char challenge[8];
	int i;

	c2c(21224983, challenge);
	hotp(challenge, "SvkZYa8LJfILarP436w{", response);
	fprintf(stdout, "challenge: %s, response: %s\n", challenge, response);

	while((opt = getopt(argc, argv, "u:s:d")) != -1) {
		switch(opt) {
			case 'u': username = optarg; break;
			case 's': secret = optarg; break;
			case 'd': debug = 1; break;
			default:
				fprintf(stderr, "unknown option\n");
		}
	}

	for(i = -5; i < 5; i++) {
		fprintf(stdout, "======= offset %d =======\n", i);
		timestamp = (time(NULL) - i*60)/60;
		fprintf(stdout, "username: %s\nsecret: %s\ndebug: %d\n", username, secret, debug);
		fprintf(stdout, "timestamp: %ld\n", timestamp);

		c2c(timestamp, challenge);
		hotp(challenge, secret, response);

		fprintf(stdout, "response: %s\n", response);
	}
}
