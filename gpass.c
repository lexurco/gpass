#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <err.h>

#include <sodium.h>

#define MAXWORDS 128
#define RANDLINE randombytes_uniform(nlines)+1
#ifndef PREFIX
#	define PREFIX "/usr/local"
#endif

FILE *dicfp;
int plen = 70;
uint32_t nlines = 0;

int
usage(void)
{
	fprintf(stderr, "%s [-d dict] [-e bits] [-n count]\n", getprogname());
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
	if (pledge("stdio unveil rpath", NULL) == -1)
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
			plen = strtonum(optarg, 1, INT_MAX, NULL);
			if (!plen)
				errx(1, "bad entropy (must be 1-%d bits)",
				    INT_MAX);
			break;
		case 'n':
			npass = strtonum(optarg, 1, INT_MAX, NULL);
			if (!npass)
				errx(1,
				    "bad number of passphrases (must be 1-%d)",
				    INT_MAX);
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
