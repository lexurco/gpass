VERSION = 0.3.2

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

INCS = -I/usr/local/include
LIBS = -L/usr/local/lib -lm -lsodium

CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -Wall -pedantic $(INCS) -O2
LDFLAGS = $(LIBS)

# Debug
#CFLAGS = -std=c99 -Wall -pedantic $(INCS) -Wextra -O0 -g

# Linux
#MANPREFIX = $(PREFIX)/share/man
#INCS =
#LIBS = -lm -lsodium
