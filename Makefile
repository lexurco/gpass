include version.mk

BIN = gpass
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)
M4S = $(BIN:=.m4)

PREFIX ?= $(DESTDIR)/usr/local
MANPREFIX ?= $(PREFIX)/man

M4 ?= m4

LIBS = -lm

bindir = $(PREFIX)/bin
mandir = $(MANPREFIX)/man1
sharedir = $(PREFIX)/share
gpassdir = $(sharedir)/gpass

all: $(BIN) $(MAN)

.SUFFIXES: .c .o .1 .m4

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS) $(LDFLAGS)

.c.o:
	$(CC) -std=c99 -pedantic -DPREFIX=\"$(PREFIX)\" $(CFLAGS) $(CPPFLAGS) -c $<

$(MAN): $(M4S)

.m4.1:
	$(M4) -DPREFIX=$(PREFIX) <$< >$@

install: all
	mkdir -p $(bindir)
	install -m 755 $(BIN) $(bindir)
	mkdir -p $(mandir)
	install -m 644 $(MAN) $(mandir)
	mkdir -p $(gpassdir)
	install -m 644 eff.long $(gpassdir)

uninstall:
	cd $(bindir) && rm -f $(BIN)
	cd $(mandir) && rm -f $(MAN)
	rm -rf $(gpassdir)

clean:
	-rm -f $(BIN) $(OBJ) $(MAN) *.tar.gz *.core gpass-$(V)

dist: clean
	mkdir -p gpass-$(V)
	cp -f CHANGES README Makefile version.mk eff.long $(SRC) $(M4S) gpass-$(V)
	tar cf - gpass-$(VERSION) | gzip >gpass-$(V).tar.gz
	rm -rf gpass-$(V)

tags:
	ctags $(SRC)

.PHONY: all options install uninstall clean dist
