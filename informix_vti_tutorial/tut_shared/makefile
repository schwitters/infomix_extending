######################################################################
# makefile for sample program
######################################################################

CREATEOBJ = gcc -Wall -fPIC -c *.c -I. 
CREATELIB = gcc -shared -Wl,-soname,libtutorial.so.1 -o libtutorial.so.1.0 *.o
CREATELINKS = chown informix:informix .; su informix -c "ln -s libtutorial.so.1.0 libtutorial.so"; su informix -c "ln -s libtutorial.so.1.0 libtutorial.so.1"
CHANGEOWNER = chown informix:informix *
COPYLIB = cp libtutorial.so.1.0 $(INFORMIXDIR)/lib/libtutorial.so.1; chown informix:informix $(INFORMIXDIR)/lib/libtutorial.so.1

all: clean compile

# SMA sample building
sample: sample.c
	$(CREATEOBJ)
	$(CREATELIB)
	$(CREATELINKS)
	$(CHANGEOWNER)
	$(COPYLIB)

compile: sample

clean:
	- rm -f sample
	- rm -f *.o
	- rm -f *.so*
	- rm -f $(INFORMIXDIR)/lib/libturorial.so.1
