static const char ident[] = "@(#) $Id: csv_fix.c,v 1.1 2005/04/13 12:48:15 richard Exp $";

/*
    'csv_fix'

    'Fixes' malformed csv-files.

    (C) Richard Harnden, 2005

    Please report bugs: <richard.harnden@lineone.net>

    This source is public domain.  Use/copy at will.  No warranties, etc.
*/

#include "unl_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 8192
#define MAXSIZE 65536

static char *progname;

static void usage(void)
{
    fprintf(
        stderr,
        "usage:\n"
        "%s [-i <filename>] [-o <filename>]\n\n"
        "where:\n"
        "   -i <filename>, take input from <filename>, default stdin.\n"
        "   -o <filename>, send output to <filename>, default stdout.\n",
        progname
    );
}

static void csv_fix(void)
{
	char_esc	*s, *x;
	char_esc	overflow[BUFSIZE];
	size_t		n = BUFSIZE;

	if	( (s = malloc(BUFSIZE)) == NULL )
	{
		perror(progname);
		exit(EXIT_FAILURE);
	}

	while ( fgets_csv(s, n, stdin) != NULL )
	{
		while ( errno == ENOMEM )
		{
			n += BUFSIZE;

			if	( n > MAXSIZE )
				break;

			if	( (x = realloc(s, n)) == NULL )
			{
				perror(progname);
				exit(EXIT_FAILURE);
			}

			s = x;
			errno = 0;

			fgets_csv(overflow, BUFSIZE, stdin);
			strcat(s, overflow);
		}

		if	( errno == ENOMEM )
		{
			fprintf(stderr, "%s: line too long!\n", progname);
			exit(EXIT_FAILURE);
		}

		if	( errno )
		{
			perror(progname);
			exit(EXIT_FAILURE);
		}

		puts_csv(s);

		if	( errno )
		{
			perror(progname);
			exit(EXIT_FAILURE);
		}
	}

	free(s);
}

int main(int argc, char **argv)
{
    int c;
    char    *ifile = NULL;
    char    *ofile = NULL;

    progname = *argv;

    while ( (c = getopt(argc, argv, "i:o:")) != EOF)
    {
        switch (c)
        {
            case 'i':
                ifile = optarg;
                break;

            case 'o':
                ofile = optarg;
                break;

            default:
                usage();
                return EXIT_FAILURE;
        }
    }

    if  ( ifile != NULL && ofile != NULL && strcmp(ifile, ofile) == 0 )
    {
        if  ( strcmp(ifile, "-") != 0 )
        {
            fprintf(stderr,
                    "%s: input and output filenames are the same!\n",
                    progname);

            return EXIT_FAILURE;
        }
    }

    if  ( ifile != NULL && strcmp(ifile, "-") != 0 )
    {
        if  ( !freopen(ifile, "r", stdin) )
        {
            perror(ifile);
            return EXIT_FAILURE;
        }
    }

    if  ( ofile != NULL && strcmp(ofile, "-") != 0 )
    {
        if  ( !freopen(ofile, "w", stdout) )
        {
            perror(ofile);
            return EXIT_FAILURE;
        }
    }

    csv_fix();

    return 0;
}

