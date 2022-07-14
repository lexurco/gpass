VERSION = 0.1

# NOTE: avoid equality signs (`=') in PREFIX, or edit the rule install.
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/man

INCS = -I/usr/local/include
LIBS = -L/usr/local/lib -lm -lsodium

CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -std=c99 -Wall -pedantic $(INCS)
LDFLAGS = $(LIBS)
CFLAGS_EXTRA = -fstack-protector-strong -O2 -fPIE
CPPFLAGS_EXTRA = -D_FORTIFY_SOURCE=2
LDFLAGS_EXTRA = -pie -Wl,-z,relro -Wl,-z,now
CFLAGS_DEBUG = -Wextra -Wformat -Wformat-security -fstack-protector-all -O0 -g
