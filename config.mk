VERSION = 0.1

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man
# Linux
#MANPREFIX = $(PREFIX)/share/man

LIBS = -lm -lsodium
# OpenBSD
#INCS = -I/usr/local/include
#LIBS = -L/usr/local/lib -lm -lsodium

CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -Wall -pedantic $(INCS) -O2
LDFLAGS = $(LIBS)

# Libbsd
#CFLAGS = -std=c99 -Wall -pedantic -O2 -DLIBBSD_OVERLAY -isystem /usr/include/bsd $(INCS)
#LDFLAGS = -lbsd $(LIBS)

# Debug
#CFLAGS = -std=c99 -Wall -pedantic $(INCS) -Wextra -O0 -g
# Libbsd+debug
#CFLAGS = -std=c99 -Wall -pedantic -DLIBBSD_OVERLAY -isystem /usr/include/bsd $(INCS) -Wextra -O0 -g
