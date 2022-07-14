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

char *pgen, *dict = NULL;
int npass = 1, minent = 70;

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
	int nlines = 0;
	FILE *fp;
	if (!(fp = fopen(dict, "r"))) {
		err("could not open the dictionary file %s", dict);
		exit(EXIT_FAILURE);
		return;
	}
	for (char c = getc(fp); c != EOF; c = getc(fp))
		nlines = (c == '\n') ? nlines + 1 : nlines;

	int plen = minent / log2(nlines) + 1;
	if (plen > nlines) {
		err("dictionary is too short");
		exit(EXIT_FAILURE);
	}
	if (plen >= MAXWORDS) {
		err("required passphrase length is too big");
		exit(EXIT_FAILURE);
	}
	int nums[MAXWORDS];
	for (int i = 0; i < plen; i++)
		nums[i] = (int)randombytes_uniform(nlines)+1;

	rewind(fp);
	nlines = 1;
	char c = getc(fp);
	for(int i = 0; i < plen; c = getc(fp)) {
		if (nlines == nums[i]) {
			for (; c != '\n' && c != EOF && c != '\0'; c = getc(fp))
				putchar(c);
			putchar(' ');
			if (++i < plen && nums[i] <= nlines) {
				rewind(fp);
				nlines = 0;
			}
		}
		nlines += (c == '\n' || c == EOF || c == '\0') ? 1 : 0;
	}
	putchar('\n');
	fclose(fp);
}

int
main(int argc, char *argv[])
{
	pgen = argv[0];
	if (sodium_init() < 0) {
		err("could not initialise libsodium");
		return 1;
	}
	int c;
	while ((c = getopt(argc, argv, "d:e:n:")) != -1) {
		switch (c) {
		case 'd':
			dict = optarg;
			break;
		case 'e':
			minent = strtonum(optarg, 1, INT_MAX, NULL);
			if (!minent) {
				err("invalid entropy: use 1-%d", INT_MAX);
				return 1;
			}
			break;
		case 'n':
			npass = strtonum(optarg, 1, INT_MAX, NULL);
			if (!npass) {
				err("invalid number of passphrases: use 1-%d",
					INT_MAX);
				return 1;
			}
			break;
		default:
			usage();
		}
	}
	dict = dict ? dict : getenv("GPASS_DIC");
	dict = dict ? dict : PREFIX "/share/gpass/eff.long";
	for (int i = 0; i < npass; i++)
		gen();
}
