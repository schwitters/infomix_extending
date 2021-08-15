########################################################################
#
# This Makefile should be executed with the Unix 'make' utility.
#
# This Makefile assumes that the INFORMIXDIR environment variable
# has been set.
#
########################################################################

.SUFFIXES:
.SUFFIXES: .ec

RM      = rm -f

ESQL    = INFORMIXC=$(CC) esql

.ec:
	$(ESQL) -g -o $@ $<
	$(RM) $@.c $@.o
	chmod 755 $@

EC_FILES = aesop.ec \
           equal.ec

PROGRAMS = $(EC_FILES:.ec=)

all: $(PROGRAMS)

clean:
	-$(RM) $(PROGRAMS)
