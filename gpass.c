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

#define MAXWORDS 128
#define RANDLINE randombytes_uniform(nlines)+1
#ifndef PREFIX
#	define PREFIX "/usr/local"
#endif

FILE *dicfp;
int plen = 70;
uint32_t nlines = 0;

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
	char c;
	int left = plen;
	uint32_t cur = 1, sought = RANDLINE;

	rewind(dicfp);

	for (;;) {
		while (getc(dicfp) != '\n')
			;
		if (++cur != sought)
			continue;
		while ((c = getc(dicfp)) != '\n')
			putchar(c);
		if (!--left)
			break;
		putchar(' ');
		if ((sought = RANDLINE) <= cur) {
			rewind(dicfp);
			cur = 0;
		}
	}

	putchar('\n');
}

int
main(int argc, char *argv[])
{
	int c, npass = 1;
	char *dicname = NULL;

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
	while ((c = getc(dicfp)) != EOF && nlines < UINT32_MAX)
		nlines += (c == '\n');
	if (nlines < 2)
		errx(1, "%s has less that 2 lines", dicname);
	int log2nlines = log2(nlines);
	plen = plen / log2nlines + !!(plen % log2nlines);
	plen += !plen;

	for (int i = 0; i < npass; i++)
		gen();
	fclose(dicfp);

	return 0;
}
