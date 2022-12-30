#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <sodium.h>

#define MAXWORDS 32768
#ifndef PREFIX
#	define PREFIX "/usr/local"
#endif

FILE *dicfp;
uint32_t nwords = 0;
int offs[MAXWORDS];
int plen = 70;

void
errx(int eval, const char *fmt, ...)
{
	fputs("gpass: ", stderr);
	if (fmt != NULL) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fputc('\n', stderr);
	exit(eval);
}

void
err(int eval, const char *fmt, ...)
{
	char *e = strerror(errno);
	fputs("gpass: ", stderr);
	if (fmt != NULL) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fputs(": ", stderr);
	fputs(e, stderr);
	fputc('\n', stderr);
	exit(eval);
}

int
usage(void)
{
	fprintf(stderr, "usage: gpass [-d dict] [-e bits] [-n count]\n");
	exit(EXIT_FAILURE);
}

void
gen(void)
{
	int c;
	int left;
	uint32_t n;

	for (left = plen; left; left--) {
		n = randombytes_uniform(nwords) + 1;
		if (fseek(dicfp, offs[n], SEEK_SET) == -1)
			err(1, "fseek");
		while ((c = getc(dicfp)) != EOF && !isspace(c))
			putchar(c);
		if (left > 1)
			putchar(' ');
	}
	putchar('\n');
}

int
main(int argc, char *argv[])
{
	int c, isword, npass, temp;
	char *dicname;

	isword = 0;
	npass = 1;
	dicname = NULL;

#ifdef __OpenBSD__
	if (pledge("stdio unveil rpath", NULL) == -1) /* first call */
		err(1, "pledge");
#endif
	if (sodium_init() < 0)
		err(1, "libsodium");

	while ((c = getopt(argc, argv, "d:e:n:")) != -1) {
		switch (c) {
		case 'd':
			dicname = optarg;
			break;
		case 'e':
			if ((plen = atoi(optarg)) < 1)
				errx(1, "less than 1 bit of entropy requested");
			break;
		case 'n':
			if ((npass = atoi(optarg)) < 1)
				errx(1, "less than 1 passphrase requested");
			break;
		default:
			usage();
		}
	}

	if (!dicname && !(dicname = getenv("GPASS_DIC")))
		dicname = PREFIX "/share/gpass/eff.long";
#ifdef __OpenBSD__
	if (unveil(dicname, "r") == -1)
		err(1, "unveil %s", dicname);
	if (pledge("stdio rpath", NULL) == -1) /* revoke unveil */
		err(1, "pledge");
#endif

	if (!(dicfp = fopen(dicname, "r")))
		err(1, "could not open %s", dicname);
	offs[0] = ftell(dicfp);
	while ((c = getc(dicfp)) != EOF && nwords < MAXWORDS)
		if (isspace(c)) {
			nwords += isword ? 1 : 0;
			isword = 0;
			offs[nwords] = ftell(dicfp);
		} else
			isword = 1;
	if (nwords < 2)
		errx(1, "%s has less that 2 words", dicname);

	temp = log2(nwords);
	plen = plen / temp + !!(plen % temp);
	plen += !plen;

	for (int i = 0; i < npass; i++)
		gen();
	fclose(dicfp);

	return 0;
}
