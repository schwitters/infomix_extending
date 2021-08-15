#**************************************************************************
#* IBM Informix Sample DataBlade Module                                   *
#**************************************************************************

CFLAGS=-g -fPIC -I$(INFORMIXDIR)/incl/public -I$(INFORMIXDIR)/extend/tutorial/tut_shared -L$(INFORMIXDIR)/extend/tutorial/tut_shared
AWKCMD='BEGIN { } { if ( ! /VPCLASS tutorial,num=1,noage/ ) print; }'

all: clean compile configure

compile:
	chown informix:informix .
	gcc $(CFLAGS) -c tutorial.c
	gcc -shared  $(CFLAGS) -o tutorial.bld tutorial.o -ltutorial
	su informix -c "ln -s $(INFORMIXDIR)/extend/tutorial/tut_shared/libtutorial.so.1.0 libtutorial.so"
	su informix -c "ln -s $(INFORMIXDIR)/extend/tutorial/tut_shared/libtutorial.so.1.0 libtutorial.so.1"
	ln -s $(INFORMIXDIR)/extend/tutorial/tut_shared/libtutorial.so.1.0 /usr/lib/libtutorial.so.1
	chown informix:informix *

configure:
	onmode -ky
	awk $(AWKCMD) $(INFORMIXDIR)/etc/$(ONCONFIG) >/tmp/onconfig
	cp /tmp/onconfig $(INFORMIXDIR)/etc/$(ONCONFIG)
	echo "VPCLASS tutorial,num=1,noage" >>$(INFORMIXDIR)/etc/$(ONCONFIG)
	chmod 555 $(INFORMIXDIR)/extend/tutorial/tutorial.bld
	@echo LD_LIBRARY_PATH = $(LD_LIBRARY_PATH)
	oninit

clean:
	rm -f *.bld *.o *.so* *~
	rm -f /usr/lib/libtutorial.so.1
