/*
 * Copyright (c) 2022 Alex Arx <aa@manpager.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define MAXWORDS 	32768
#define NNUM		('9'-'0' + 1)
#define NALPHA		('z'-'a' + 1)

int aflag = 0;

static int
usage(void)
{
	fprintf(stderr,
"usage: gpass [-a] [-d dict] [-e bits] [-l length] [-n num]\n");
	exit(EXIT_FAILURE);
}

static int
getplen(int n, int ent) {
	long	temp;
	int	plen;

	temp = log2(n);
	plen = ent / temp + !!(ent % temp);
	return (plen > 0) ? plen : 1;
}

static void
genalpha(int plen, int max)
{
	int		c;
	int		left;
	uint32_t	n;

	for (left = plen; left; left--) {
		n = arc4random_uniform(max);
		if (n < NNUM)
			putchar('0' + n);
		else if (n < NNUM + NALPHA)
			putchar('A' + (n - NNUM));
		else
			putchar('a' + (n - NNUM - NALPHA));
	}
	putchar('\n');
}

static void
gpass_alpha(int npass, int plen, int ent)
{
	int	n = NNUM + NALPHA*2;

#ifdef __OpenBSD__
	if (pledge("stdio rpath", NULL) == -1) /* revoke unveil */
		err(1, "pledge");
#endif
	if (plen == 0)
		plen = getplen(n, ent);
	for (int i = 0; i < npass; i++)
		genalpha(plen, n);
}

static void
genwords(int plen, int max, const long *offs, FILE *fp)
{
	int		c;
	unsigned int	n, left;

	for (left = plen; left > 0; left--) {
		n = arc4random_uniform(max);
		if (fseek(fp, offs[n], SEEK_SET) == -1)
			err(1, "fseek");
		while ((c = getc(fp)) != EOF && isgraph(c))
			putchar(c);
		if (left > 1)
			putchar(' ');
	}
	putchar('\n');
}

static void
gpass_words(int npass, int plen, int ent, char *f)
{
	FILE	*fp;
	long	 offs[MAXWORDS];
	int	 c, isword = 0, nwords = 0;

#ifdef __OpenBSD__
	if (unveil(f, "r") == -1)
		err(1, "unveil %s", f);
	if (pledge("stdio rpath", NULL) == -1) /* revoke unveil */
		err(1, "pledge");
#endif
	if (!(fp = fopen(f, "r")))
		err(1, "could not open %s", f);
	while ((c = getc(fp)) != EOF && nwords < MAXWORDS)
		if (!isgraph(c))
			isword = 0;
		else if (!isword) {
			isword = 1;
			offs[nwords++] = ftell(fp) - 1;
		}
	if (nwords < 2)
		errx(1, "%s has less that 2 words", f);
	if (plen == 0)
		plen = getplen(nwords, ent);
	for (int i = 0; i < npass; i++)
		genwords(plen, nwords, offs, fp);
	fclose(fp);
}

static char *
getdicname(int *allocated)
{
	char	*xdg, *home, *path;

	if ((path = getenv("GPASS_DIC")) != NULL && *path != '\0') {
		*allocated = 0;
		return path;
	}

	if (((xdg = getenv("XDG_DATA_HOME")) != NULL && *xdg != '\0') ||
	    ((home = getenv("HOME")) != NULL && *home != '\0')) {
		size_t	 sz;
		char	*pre, *suf;

		if (xdg != NULL) {
			pre = xdg;
			suf = "/gpass.dic";
		} else {
			pre = home;
			suf = "/.gpass.dic";
		}

		sz = strlen(pre) + strlen(suf) + 1;
		if ((path = malloc(sz)) == NULL)
			err(1, "malloc");
		strlcpy(path, pre, sz);
		strlcat(path, suf, sz);

		if (access(path, F_OK) == 0) {
			*allocated = 1;
			return path;
		} else {
			free(path);
			path = NULL;
		}
	}

	if (access("/usr/share/gpass.dic", F_OK) == 0) {
		*allocated = 0;
		return "/usr/share/gpass.dic";
	}
	if (access("/usr/local/share/gpass.dic", F_OK) == 0) {
		*allocated = 0;
		return "/usr/local/share/gpass.dic";
	}

	*allocated = 0;
	return NULL;
}

int
main(int argc, char *argv[])
{
	int		 c, ent = 70, npass = 1, plen = 0;
	char		*dicname = NULL;
	const char	*errstr = NULL;

#ifdef __OpenBSD__
	if (pledge("stdio unveil rpath", NULL) == -1) /* first call */
		err(1, "pledge");
#endif
	while ((c = getopt(argc, argv, "ad:e:l:n:")) != -1) {
		switch (c) {
		case 'a':
			aflag = 1;
			break;
		case 'd':
			dicname = optarg;
			break;
		case 'e':
			ent = strtonum(optarg, 0, INT_MAX, &errstr);
			if (errstr != NULL)
				errx(1, "entropy is %s: %s", errstr, optarg);
			break;
		case 'l':
			plen = strtonum(optarg, 0, INT_MAX, &errstr);
			if (errstr != NULL)
				errx(1, "length is %s: %s", errstr, optarg);
			break;
		case 'n':
			npass = strtonum(optarg, 0, INT_MAX, &errstr);
			if (errstr != NULL)
				errx(1, "npass is %s: %s", errstr, optarg);
			break;
		default:
			usage();
		}
	}

	if (npass == 0 || (plen == 0 && ent == 0))
		return 0;

	if (aflag)
		gpass_alpha(npass, plen, ent);
	else {
		int allocated = 0;
		if (dicname == NULL &&
		    (dicname = getdicname(&allocated)) == NULL)
			errx(1, "couldn't find dictionary file");
		gpass_words(npass, plen, ent, dicname);
		if (allocated)
			free(dicname);
	}

	return 0;
}
