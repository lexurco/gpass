include version.mk

BIN = gpass
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

PREFIX ?= $(DESTDIR)/usr/local
MANPREFIX ?= $(PREFIX)/man

LIBS = -lm

bindir = $(PREFIX)/bin
man1dir = $(MANPREFIX)/man1
sharedir = $(PREFIX)/share
gpassdir = $(sharedir)/gpass

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS) $(LDFLAGS)

.c.o:
	$(CC) -std=c99 -pedantic -DPREFIX=\"$(PREFIX)\" $(CFLAGS) $(CPPFLAGS) -c $<

install: all
	mkdir -p $(bindir)
	install -m 755 $(BIN) $(bindir)
	mkdir -p $(man1dir)
	sed s=PREFIX=$(PREFIX)=g <gpass.1 >$(man1dir)/gpass.1
	cd $(man1dir) && chmod 644 $(MAN)
	mkdir -p $(gpassdir)
	install -m 644 eff.long $(gpassdir)

uninstall:
	cd $(bindir) && rm -f $(BIN)
	cd $(man1dir) && rm -f $(MAN)
	rm -rf $(gpassdir)

clean:
	-rm -f $(BIN) $(OBJ) *.tar.gz *.core gpass-$(V)

dist: clean
	mkdir -p gpass-$(V)
	cp -f CHANGES README Makefile version.mk eff.long $(SRC) $(MAN) gpass-$(V)
	tar cf - gpass-$(VERSION) | gzip >gpass-$(V).tar.gz
	rm -rf gpass-$(V)

tags:
	ctags $(SRC)

.PHONY: all options install uninstall clean dist
