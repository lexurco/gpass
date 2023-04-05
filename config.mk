VERSION = 0.3.4

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -Wall -pedantic -O2
LDFLAGS = -lm

# Linux
#MANPREFIX = $(PREFIX)/share/man

# Debug
#CFLAGS = -std=c99 -Wall -pedantic -Wextra -O0 -g
