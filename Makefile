include config.mk

.SUFFIXES: .o .c

BIN = gpass
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

all: options $(BIN)

options:
	@echo gpass build options:
	@echo "	CFLAGS = $(CFLAGS)"
	@echo "	LDFLAGS = $(LDFLAGS)"
	@echo "	CC = $(CC)"

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ): config.mk

.c.o:
	$(CC) -DPREFIX=\"$(PREFIX)\" $(CFLAGS) $(CPPFLAGS) -c $<

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed s=PREFIX=$(PREFIX)=g <gpass.1 >$(DESTDIR)$(MANPREFIX)/man1/gpass.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/gpass.1
	mkdir -p $(DESTDIR)$(PREFIX)/share/gpass
	install -m 644 eff.long $(DESTDIR)$(PREFIX)/share/gpass

uninstall:
	cd $(DESTDIR)$(PREFIX)/bin && rm -f $(BIN)
	cd $(DESTDIR)$(MANPREFIX)/man1 && rm -f $(MAN)
	cd $(DESTDIR)$(PREFIX)/share && rm -rf gpass

clean:
	-rm -f $(BIN) $(OBJ) *.tar.gz *.core

dist: clean
	mkdir -p gpass-$(VERSION)
	cp -f README Makefile config.mk eff.long $(SRC) $(MAN) gpass-$(VERSION)
	tar cf - gpass-$(VERSION) | gzip >gpass-$(VERSION).tar.gz
	rm -rf gpass-$(VERSION)

.PHONY: all options install uninstall clean dist
