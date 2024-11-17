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

#define MAXWORDS 32768
#ifndef PREFIX
#	define PREFIX "/usr/local"
#endif

#define NNUM ('9'-'0' + 1)
#define NALPHA ('z'-'a' + 1)

FILE *dicfp;
uint32_t nwords = 0;
int offs[MAXWORDS];
int plen = 70;
int aflag = 0;

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
	fprintf(stderr, "usage: gpass [-a] [-d dict] [-e bits] [-n count]\n");
	exit(EXIT_FAILURE);
}

void
setplen(void) {
	int temp;

	temp = log2(nwords);
	plen = plen / temp + !!(plen % temp);
	plen += !plen;
}

void
genalpha(void)
{
	int c;
	int left;
	uint32_t n;

	for (left = plen; left; left--) {
		n = arc4random_uniform(NNUM + NALPHA*2);
		if (n < NNUM)
			putchar('0' + n);
		else if (n < NNUM + NALPHA)
			putchar('A' + (n - NNUM));
		else
			putchar('a' + (n - NNUM - NALPHA));
	}
	putchar('\n');
}

void
gpass_alpha(int npass)
{
#ifdef __OpenBSD__
	if (pledge("stdio rpath", NULL) == -1) /* revoke unveil */
		err(1, "pledge");
#endif
	nwords = NNUM + NALPHA*2;
	setplen();
	for (int i = 0; i < npass; i++)
		genalpha();
}

void
genwords(void)
{
	int c;
	int left;
	uint32_t n;

	for (left = plen; left; left--) {
		n = arc4random_uniform(nwords) + 1;
		if (fseek(dicfp, offs[n], SEEK_SET) == -1)
			err(1, "fseek");
		while ((c = getc(dicfp)) != EOF && !isspace(c))
			putchar(c);
		if (left > 1)
			putchar(' ');
	}
	putchar('\n');
}

void
gpass_words(int npass, char *f)
{
	char c, isword;

#ifdef __OpenBSD__
	if (unveil(f, "r") == -1)
		err(1, "unveil %s", f);
	if (pledge("stdio rpath", NULL) == -1) /* revoke unveil */
		err(1, "pledge");
#endif
	if (!(dicfp = fopen(f, "r")))
		err(1, "could not open %s", f);
	offs[0] = ftell(dicfp);
	while ((c = getc(dicfp)) != EOF && nwords < MAXWORDS)
		if (isspace(c)) {
			nwords += isword ? 1 : 0;
			isword = 0;
			offs[nwords] = ftell(dicfp);
		} else
			isword = 1;
	if (nwords < 2)
		errx(1, "%s has less that 2 words", f);
	setplen();
	for (int i = 0; i < npass; i++)
		genwords();
	fclose(dicfp);
}

int
main(int argc, char *argv[])
{
	int c, npass;
	char *dicname;

	npass = 1;
	dicname = NULL;

#ifdef __OpenBSD__
	if (pledge("stdio unveil rpath", NULL) == -1) /* first call */
		err(1, "pledge");
#endif
	while ((c = getopt(argc, argv, "ad:e:n:")) != -1) {
		switch (c) {
		case 'a':
			aflag = 1;
			break;
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

	if (aflag)
		gpass_alpha(npass);
	else {
		if (!dicname && !(dicname = getenv("GPASS_DIC")))
			dicname = PREFIX "/share/gpass/eff.long";
		gpass_words(npass, dicname);
	}

	return 0;
}
