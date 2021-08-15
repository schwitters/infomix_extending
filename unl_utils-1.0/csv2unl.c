static const char ident[] = "@(#) $Id: csv2unl.c,v 1.2 2005/04/13 12:55:32 richard Exp $";

/*
    'csv2unl'

    Converts csv-file into an Informix unl-file.

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
        "%s [-i <filename>] [-o <filename>] [-d <delimiter>] "
        "[-h] [-F <cols>]\n\n"
        "where:\n"
        "   -i <filename>, take input from <filename>, default stdin.\n"
        "   -o <filename>, send output to <filename>, default stdout.\n"
        "   -d <delimiter>, use <delimiter> as the unl-delimiter,\n"
        "                   defaults to $DBDELIMITER or '|'.\n"
        "   -h, the first line in the csv-file are headings.\n"
        "   -F <ncols>, force output to at least <ncols> columns.\n",
        progname
    );
}

static void csv2unl(int headings, int ncols, char delim)
{
	char_esc	*s, *x;
	char_esc	overflow[BUFSIZE];
	size_t		n = BUFSIZE;
	int			cols;

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

		if	( headings )
		{
			if	( ncols == -1 )
				ncols = count_cols(s);

			headings = 0;

			continue;
		}

		fputs_unl(s, stdout);

		if	( errno )
		{
			perror(progname);
			exit(EXIT_FAILURE);
		}

        if  ( ncols > 0 )
        {
            for (cols=count_cols(s);cols<ncols;cols++)
                    fputc(delim, stdout);
        }

        fputc('\n', stdout);
	}

	free(s);
}

int main(int argc, char **argv)
{
    int c;
    int headings = 0;
    char    *ifile = NULL;
    char    *ofile = NULL;
    char    *forcecols = NULL;
    char    *endptr;
    int     ncols;
    char    *dbdelimiter;
    char    delim;

    progname = *argv;

    if  ( (dbdelimiter = getenv("DBDELIMITER")) == NULL )
        delim = '|';
    else
        delim = *dbdelimiter;

    while ( (c = getopt(argc, argv, "hi:o:d:F:")) != EOF)
    {
        switch (c)
        {
            case 'h':
                headings = 1;
                break;

            case 'i':
                ifile = optarg;
                break;

            case 'o':
                ofile = optarg;
                break;

            case 'F':
                forcecols = optarg;
                break;

            case 'd':
                delim = *optarg;
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

    if  ( forcecols == NULL )
    {
        if  ( headings )
            ncols = -1;
        else
            ncols = 0;
    }
    else
    {
        ncols = strtol(forcecols, &endptr, 10);

        if  ( *endptr != '\0' )
        {
            fprintf(stderr, "%s: not a number: %s\n", progname, forcecols);
            return EXIT_FAILURE;
        }
    }

    if  ( !set_unl_delimiter(delim) )
    {
            fprintf(stderr,
                    "%s: invalid delimiter '%c'\n",
                    progname, delim);

            return EXIT_FAILURE;
    }

    csv2unl(headings, ncols, delim);

    return 0;
}
