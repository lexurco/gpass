include version.mk

BIN = gpass
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

PREFIX ?= $(DESTDIR)/usr/local
MANPREFIX ?= $(PREFIX)/man

LIBS = -lm

bindir = $(PREFIX)/bin
mandir = $(MANPREFIX)/man1
sharedir = $(PREFIX)/share
gpassdir = $(sharedir)/gpass

all: $(BIN) $(MAN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS) $(LDFLAGS)

.c.o:
	$(CC) -std=c99 -pedantic -DPREFIX=\"$(PREFIX)\" $(CFLAGS) $(CPPFLAGS) -c $<

install: all
	mkdir -p $(bindir)
	install -m 755 $(BIN) $(bindir)
	mkdir -p $(mandir)
	install -m 644 $(MAN) $(mandir)
	install -m 644 eff.long $(sharedir)/gpass.dic
	rm -rf $(gpassdir)	# clean up legacy files

uninstall:
	cd $(bindir) && rm -f $(BIN)
	cd $(mandir) && rm -f $(MAN)
	rm -rf $(gpassdir)	# clean up legacy files

clean:
	-rm -rf $(BIN) $(OBJ) *.tar.gz *.core gpass-$(V)

dist: clean
	mkdir -p gpass-$(V)
	cp -f CHANGES README Makefile version.mk eff.long $(SRC) $(MAN) \
	    gpass-$(V)
	tar cf - gpass-$(V) | gzip >gpass-$(V).tar.gz
	rm -rf gpass-$(V)

tags:
	ctags $(SRC)

.PHONY: all options install uninstall clean dist
