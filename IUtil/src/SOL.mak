
PROJECT_TITLE=iutil
CC=cc
COMPILE=-I$(INFORMIXDIR)/incl/public -c
CFLAGS= -g $(COMPILE)
BINDIR=./SOL
PRODDIR=./bin

OBJ1=$(BINDIR)/ascii.o

OBJS_ALL=$(OBJ1) $(OBJ2) $(OBJ3)

target: $(BINDIR)
	make -f SOL.mak server

server: $(PRODDIR)/$(PROJECT_TITLE).bld

.c.o:
	cc -DMI_SERVBUILD $(COMPILE) $(CFLAGS) $<

$(BINDIR)/ascii.o: ascii.c
	$(CC) $(CFLAGS) -o $*.o -c $?

$(PRODDIR)/$(PROJECT_TITLE).bld: $(OBJS_ALL)
	ld -G -o $(PRODDIR)/$(PROJECT_TITLE).bld $(OBJS_ALL)
	chmod a+x $(PRODDIR)/$(PROJECT_TITLE).bld

clean:
	rm $(BINDIR)/*.o $(PRODDIR)/$(PROJECT_TITLE).bld 

$(BINDIR):
	mkdir $(BINDIR)

$(PRODDIR):
	mkdir $(PRODDIR)
