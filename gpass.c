#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sodium.h>

#define MAXWORDS 128
#ifndef PREFIX
 #define PREFIX "/usr/local"
#endif

char *pgen, *dictname = NULL;
int plen = 70, nlines = 0, npass = 1;
FILE *dictfp;

#define RANDLINE (int)randombytes_uniform(nlines)+1

int
usage(void)
{
	fprintf(stderr, "%s [-d dict] [-e bits] [-n count]\n", pgen);
	exit(EXIT_FAILURE);
}

void
err(char *fmt, ...)
{
	fprintf(stderr, "%s: ", pgen);
	if (fmt) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fprintf(stderr, "\n");
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
	pgen = argv[0];
	if (sodium_init() < 0) {
		err("could not initialise libsodium");
		exit(EXIT_FAILURE);
	}
	int c;
	while ((c = getopt(argc, argv, "d:e:n:")) != -1) {
		switch (c) {
		case 'd':
			dictname = optarg;
			break;
		case 'e':
			plen = strtonum(optarg, 1, INT_MAX, NULL);
			if (!plen) {
				err("invalid entropy: use 1-%d", INT_MAX);
				exit(EXIT_FAILURE);
			}
			break;
		case 'n':
			npass = strtonum(optarg, 1, INT_MAX, NULL);
			if (!npass) {
				err("invalid number of passphrases: use 1-%d",
				    INT_MAX);
				exit(EXIT_FAILURE);
			}
			break;
		default:
			usage();
		}
	}

	if (!dictname && !(dictname = getenv("GPASS_DIC")))
		dictname = PREFIX "/share/gpass/eff.long";
	if (!(dictfp = fopen(dictname, "r"))) {
		err("could not open the dictionary file %s", dictfp);
		exit(EXIT_FAILURE);
	}
	for (char c = getc(dictfp); c != EOF; c = getc(dictfp))
		nlines += (c == '\n');
	int log2nlines = log2(nlines);
	plen = plen / log2nlines + !!(plen % log2nlines);
	plen += !plen;

	for (int i = 0; i < npass; i++)
		gen();
	fclose(dictfp);
}
