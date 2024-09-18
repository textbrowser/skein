UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
	MAKEFILE=Makefile.macos
else ifeq ($(UNAME), FreeBSD)
	MAKEFILE=Makefile.freebsd
else ifeq ($(UNAME), Linux)
	MAKEFILE=Makefile.linux
else
	MAKEFILE=Makefile.windows
endif

all:
	$(MAKE) -f $(MAKEFILE)

clean:
	$(MAKE) -f $(MAKEFILE) clean

distclean: clean

library:
	$(MAKE) -f $(MAKEFILE) library

purge:
	$(MAKE) -f $(MAKEFILE) purge
