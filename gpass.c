#include <err.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <sodium.h>

#define MAXWORDS 128
#ifndef PREFIX
 #define PREFIX "/usr/local"
#endif

char *dictname = NULL;
int plen = 70, nlines = 0, npass = 1;
FILE *dictfp;

#define RANDLINE (int)randombytes_uniform(nlines)+1

int
usage(void)
{
	fprintf(stderr, "%s [-d dict] [-e bits] [-n count]\n", getprogname());
	exit(EXIT_FAILURE);
}

void
gen(void)
{
	rewind(dictfp);
	char c;
	int left = plen, cur = 1, sought = RANDLINE;
	for (;;) {
		c = getc(dictfp);
		if (cur == sought) {
			for (; c != '\n' && c != EOF; c = getc(dictfp))
				putchar(c);
			if (--left) {
				putchar(' ');
				sought = RANDLINE;
				if (sought <= cur) {
					rewind(dictfp);
					cur = 0;
				}
			} else
				break;
		}
		cur += (c == '\n' || c == EOF || c == '\0');
	}
	putchar('\n');
}

int
main(int argc, char *argv[])
{
	if (sodium_init() < 0)
		err(1, "libsodium");

	int c;
	while ((c = getopt(argc, argv, "d:e:n:")) != -1) {
		switch (c) {
		case 'd':
			dictname = optarg;
			break;
		case 'e':
			plen = strtonum(optarg, 1, INT_MAX, NULL);
			if (!plen)
				err(1, "bad entropy");
			break;
		case 'n':
			npass = strtonum(optarg, 1, INT_MAX, NULL);
			if (!npass)
				err(1, "bad number of passphrases");
			break;
		default:
			usage();
		}
	}

	if (!dictname && !(dictname = getenv("GPASS_DIC")))
		dictname = PREFIX "/share/gpass/eff.long";
	if (!(dictfp = fopen(dictname, "r")))
		err(1, "could not open the dictionary file %s", dictname);
	for (char c = getc(dictfp); c != EOF; c = getc(dictfp))
		nlines += (c == '\n');
	if (nlines < 2)
		errx(1, "the dictionary %s has less that 2 words", dictname);
	int log2nlines = log2(nlines);
	plen = plen / log2nlines + !!(plen % log2nlines);
	plen += !plen;

	for (int i = 0; i < npass; i++)
		gen();
	fclose(dictfp);

	return 0;
}
