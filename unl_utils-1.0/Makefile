# "@(#) $Id: Makefile,v 1.1 2005/04/13 12:44:37 richard Exp $"

default : all

unl_utils.o: unl_utils.c unl_utils.h
	${CC} ${CFLAGS} -c unl_utils.c -o unl_utils.o

csv2unl : csv2unl.o unl_utils.o
	${CC} ${CFLAGS} csv2unl.o unl_utils.o -o csv2unl

unl2csv : unl2csv.o unl_utils.o
	${CC} ${CFLAGS} unl2csv.o unl_utils.o -o unl2csv

csv_fix : csv_fix.o unl_utils.o
	${CC} ${CFLAGS} csv_fix.o unl_utils.o -o csv_fix

unl2err : unl2err.o unl_utils.o
	${CC} ${CFLAGS} unl2err.o unl_utils.o -o unl2err

all: csv2unl unl2csv csv_fix unl2err

clean:
	/bin/rm csv2unl.o unl2csv.o csv_fix.o unl2err.o unl_utils.o 

realclean: clean
	/bin/rm csv2unl unl2csv csv_fix unl2err

csv2unl.o: csv2unl.c unl_utils.h
csv_fix.o: csv_fix.c unl_utils.h
unl2csv.o: unl2csv.c unl_utils.h
unl2err.o: unl2err.c unl_utils.h
