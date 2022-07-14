include config.mk

.SUFFIXES: .o .c

BIN = gpass
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LDFLAGS_EXTRA)

$(OBJ): config.mk

.c.o:
	$(CC) -DPREFIX=\"$(PREFIX)\"\
		$(CFLAGS) $(CFLAGS_EXTRA) $(CPPFLAGS) $(CPPFLAGS_EXTRA) -c $<

debug: clean
	$(MAKE)\
		CFLAGS_EXTRA='$(CFLAGS_EXTRA) $(CFLAGS_DEBUG)'\
		CPPFLAGS_EXTRA='$(CPPFLAGS_EXTRA) $(CPPFLAGS_DEBUG)'\
		LDFLAGS_EXTRA='$(LDFLAGS_EXTRA) $(LDFLAGS_DEBUG)'

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed s=/usr/local=$(PREFIX)=g gpass.1\
		>$(DESTDIR)$(MANPREFIX)/man1/gpass.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/gpass.1
	mkdir -p $(DESTDIR)$(PREFIX)/share/gpass
	install -m 644 eff.long $(DESTDIR)$(PREFIX)/share/gpass

uninstall:
	cd $(DESTDIR)$(PREFIX)/bin && rm -f $(BIN)
	cd $(DESTDIR)$(MANPREFIX)/man1 && rm -f $(MAN)
	cd $(DESTDIR)$(PREFIX)/share && rm -rf gpass

dist: clean
	mkdir -p gpass-$(VERSION)
	cp -rf README Makefile config.mk eff.long $(SRC) $(MAN) gpass-$(VERSION)
	tar -czf gpass-$(VERSION).tar.gz gpass-$(VERSION)
	rm -rf gpass-$(VERSION)

clean:
	-rm -f $(BIN) $(OBJ)
	-rm -f gpass-$(VERSION).tar.gz
	-rm -f *.core

.PHONY: all debug install uninstall clean dist
