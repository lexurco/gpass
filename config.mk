VERSION = 0.3

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man
# Linux
#MANPREFIX = $(PREFIX)/share/man

LIBS = -lm -lsodium
# BSD
#INCS = -I/usr/local/include
#LIBS = -L/usr/local/lib -lm -lsodium

CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -Wall -pedantic $(INCS) -O2
LDFLAGS = $(LIBS)

# Debug
#CFLAGS = -std=c99 -Wall -pedantic $(INCS) -Wextra -O0 -g
