static const char ident[] = "@(#) $Id: unl_utils.c,v 1.1 2005/04/13 12:46:06 richard Exp $";

/*
    'unl_utils.c'

    unl/csv file utilities. See unl_utils.h

    (C) Richard Harnden, 2005

    Please report bugs: <richard.harnden@lineone.net>

    This source is public domain.  Use/copy at will.  No warranties, etc.
*/

#include "unl_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define ESC '\\'

#define ADD_C(c, s, n)  do {*s++ = c; n++;} while (0)

static char unl_delim = '|';

struct csv_buffer
{
    enum quote_state {reset, nested1, nested2, literal} quote_state;
};

struct unl_buffer
{
    char    delimiter;
    enum escaped {no, yes} escaped;
};

static void find_col(   const char_esc *s,
                        int colno,
                        const char_esc **start,
                        const char_esc **end
                )
{
    int e = 0;
    int curr_col = 0;

    if  ( colno < 0 )
    {
        errno = ERANGE;

        *start = NULL;
        *end = NULL;
    }

    if  ( colno == 0 )
        *start = s;

    while (*s)
    {
        if  ( !e && *s == ESC )
        {
            e = 1;
            s++;
            continue;
        }

        if  ( !e && *s == '|' )
        {
            curr_col++;

            if  ( curr_col == colno )
                *start = s + 1;

            if  ( curr_col > colno )
            {
                *end = s;
                break;
            }
        }

        s++;
        e = 0;
    }

    if  ( curr_col < colno )
    {
        errno = ERANGE;

        *start = NULL;
        *end = NULL;
    }
}

unl_buffer  *new_unl_buffer(char delimiter)
{
    unl_buffer  *unlbuf;

    unlbuf = malloc(sizeof *unlbuf);

    if  ( unlbuf == NULL )
    {
        perror("unl_utils:new_unl_buffer: Cannot malloc!");
        exit(EXIT_FAILURE);
    }

    unlbuf->delimiter = delimiter;
    unlbuf->escaped = no;

    return unlbuf;
}

void    delete_unl_buffer(unl_buffer *unlbuf)
{
    free(unlbuf);
}

csv_buffer  *new_csv_buffer(void)
{
    csv_buffer  *csvbuf;

    csvbuf = malloc(sizeof *csvbuf);

    if  ( csvbuf == NULL )
    {
        perror("unl_utils:new_csv_buffer: Cannot malloc!");
        exit(EXIT_FAILURE);
    }

    csvbuf->quote_state = reset;

    return csvbuf;
}

void    delete_csv_buffer(csv_buffer *csvbuf)
{
    free(csvbuf);
}

int set_unl_delimiter(char delim)
{
    static const char   invalid[] = "0123456789abcdefABCDEF\n\\";

    if  ( strchr(invalid, delim) != NULL )
        return 0;

    unl_delim = delim;

    return 1;
}

char_esc    *fgets_csv(char_esc *s, size_t n, FILE *stream)
{
    static csv_buffer csvbuf = {reset};

    return fgets_csv_r(s, n, stream, &csvbuf);
}

char_esc    *fgets_csv_r(char_esc *s, size_t n, FILE *stream, csv_buffer *csvb\
uf)
{
    char_esc    *w = s;
    int c;
    size_t z = 2;

    while ( (c = fgetc(stream)) != EOF )
    {
        if  ( z == n ) break;

        if  ( csvbuf->quote_state == reset )
        {
            if  ( c == '"' )
            {
                csvbuf->quote_state = nested1;
                continue;
            }
            else
                csvbuf->quote_state = literal;
        }

        if  ( csvbuf->quote_state == nested1 && c == '"' )
        {
            csvbuf->quote_state = nested2;
            continue;
        }

        if  ( csvbuf->quote_state == nested2 )
        {
            if  ( c == '"' )
            {
                ADD_C(c, w, z);

                csvbuf->quote_state = nested1;
                continue;
            }
            else
                csvbuf->quote_state = literal;
        }

        if  ( csvbuf->quote_state == nested1 )
        {
            if  ( c == ',' )
            {
                ADD_C(',', w, z);
                continue;
            }
        }

        if  ( c == '\n' )
        {
            if  ( csvbuf->quote_state == nested1 )
            {
                /* this line continues */

                ADD_C('\n', w, z);

                csvbuf->quote_state = nested1;
                continue;
            }

            break;
        }

        if  ( c == '|' || c == ESC )
        {
			if	( (z+2) >= n )
			{
				z = n;
				break;
			}

            ADD_C(ESC, w, z);
			ADD_C(c, w, z);

			continue;
        }

        if  ( c == ',' )
            ADD_C('|', w, z);
        else
            ADD_C(c, w, z);

        if  ( csvbuf->quote_state == literal && c == ',' )
            csvbuf->quote_state = reset;
    }

    if  ( w == s && c == EOF )
        return NULL;

    if  ( z == n )
    {
        if  ( c != EOF || c != '\n' )
        {
            errno = ENOMEM; /* input buffer is too small */
            ungetc(c, stream);
            *w = '\0';
            return s;
        }
    }

    *w = '|';
    w++;
    *w = '\0';

    csvbuf->quote_state = reset;

    return s;
}

char_esc    *fgets_unl(char_esc *s, size_t n, FILE *stream)
{
    static unl_buffer unlbuf = {'|', no};

    unlbuf.delimiter = unl_delim;

    return  fgets_unl_r(s, n, stream, &unlbuf);
}

char_esc    *fgets_unl_r(char_esc *s, size_t n, FILE *stream, unl_buffer *unlb\
uf)
{
    char_esc    *w = s;
    int     c;
    size_t  z = 1;

    while ( (c = fgetc(stream)) != EOF )
    {
        if  ( z == n ) break;

        if  ( unlbuf->escaped == no && c == ESC )
        {
            unlbuf->escaped = yes;
            continue;
        }

        if  ( unlbuf->escaped == yes )
        {
            if  ( (c == unlbuf->delimiter && c != '|') || c == '\n' )
            {
                ADD_C(c, w, z);

                unlbuf->escaped = no;
                continue;
            }

			if	( (z+2) >= n )
			{
				z = n;
				break;
			}

            ADD_C(ESC, w, z);
            ADD_C(c, w, z);

            unlbuf->escaped = no;
            continue;
        }

        if  ( c == '|' && unlbuf->delimiter != '|' )
        {
            ADD_C(ESC, w, z);

            if  ( z == n ) break;
        }

        if  ( c == '\n' ) break;

        if  ( c == unlbuf->delimiter )
            c = '|';

        ADD_C(c, w, z);
    }

    *w = '\0';

    if  ( w == s && c == EOF )
        return NULL;

    if  ( z == n )
    {
        if  ( c != EOF || c != '\n' )
        {
            errno = ENOMEM; /* input buffer is too small */
            ungetc(c, stream);

            return s;
        }
    }

    return s;
}

int fputs_csv(const char_esc *s, FILE *stream)
{
    int n = 0;
    const char_esc *w;
    int q;
    int e;

    while ( *s )
    {
        /* needs quoting? ...*/
        w = s;
        q = 0;
        e = 0;

        while ( *w )
        {
            if  ( *w == '"' || *w == ',' || *w == '\n' )
            {
                q = 1;
                break;
            }

            if  ( !e && *w == ESC )
            {
                e = 1;
                w++;
                continue;
            }

            if  ( !e && *w == '|' )
                break;

            e = 0;
            w++;
        }
        /* ... needs quoting? */

        /* output ...*/
        if  ( q )
        {
            fputc('"', stream);
            n++;
        }

        w = s;

        while ( *w )
        {
            if  ( q && *w == '"' )
            {
                fputs("\"\"", stream);
                n += 2;
                w++;
                continue;
            }

            if  ( !e && *w == ESC )
            {
                w++;
                e = 1;
                continue;
            }

            if  ( !e && *w == '|' )
                break;

            e = 0;

            fputc(*w, stream);
            w++;
            n++;
        }

        if  ( q )
        {
            fputc('"', stream);
            n++;
        }
        /* ... output */

        w++;
        if  ( *w )
        {
            fputc(',', stream);
            n++;
        }
        else
            break;

        s = w;
    }

    return n;
}

int puts_csv(const char_esc *s)
{
    int n;

    n = fputs_csv(s, stdout) + 1;
    fputc('\n', stdout);

    return n;
}

int fputs_unl(const char_esc *s, FILE *stream)
{
    return fputs_unl_r(s, stream, unl_delim);
}

int fputs_unl_r(const char_esc *s, FILE *stream, char delimiter)
{
    int n = 0;
    int e = 0;

    while (*s)
    {
        if  ( !e && *s == ESC )
        {
            e = 1;
            s++;
            continue;
        }

        if  ( e )
        {
            if  ( *s == '|' )
            {
                if  ( delimiter == '|' )
                {
                    fputc(ESC, stream);
                    n++;
                }

                fputc('|', stream);
                n++;
            }

            if  ( *s == ESC || *s == ' ' )
            {
                fputc(ESC, stream);
                fputc(*s, stream);
                n += 2;
            }

            e = 0;
            s++;

            continue;
        }

        if  ( *s == '|' )
        {
            fputc(delimiter, stream);
            n++;
            s++;
            continue;
        };

        if  ( *s == '\n' || *s == delimiter )
        {
            fputc(ESC, stream);
            n++;
        }

        fputc(*s, stream);
        n++;

        s++;
    }

    return n;
}

int puts_unl(const char_esc *s)
{
    return puts_unl_r(s, unl_delim);
}

int puts_unl_r(const char_esc *s, char delimiter)
{
    int n;

    n = fputs_unl_r(s, stdout, delimiter) + 1;
    fputc('\n', stdout);

    return n;
}

int count_cols(const char_esc *s)
{
    int n = 0;
    int e = 0;

    while (*s)
    {
        if  ( !e && *s == ESC )
        {
            e = 1;
            s++;
            continue;
        }

        if  ( !e && *s == '|' ) n++;

        s++;
        e = 0;
    }

    return n;
}

size_t get_col(const char_esc *s, int colno, char *col, size_t n)
{
    const char_esc  *a, *z;
    int e = 0;
    size_t  r = 0;

    find_col(s, colno, &a, &z);

    if  ( a == NULL || z == NULL )
        return 0;

    while ( a < z )
    {
        if  ( !e && *a == ESC )
        {
            e = 1;
            a++;
            continue;
        }

        r++;

        if  ( col && r < n )
        {
            *col = *a;
            col++;
        }

        a++;
        e = 0;
    }

    if (col) *col = '\0';
    r++;

    return r;
}

size_t  col2csv(const char *col, char *csv, size_t n)
{
    size_t r = 0;
    const char *s = col;
    int q = 0;

    while (*s)
    {
        if  ( *s == '"' || *s == ',' || *s == '\n' )
        {
            q = 1;
            break;
        }

        s++;
    }

    s = col;

    if  (q)
    {
        r++;

        if  ( csv && r < n )
        {
            *csv = '"';
            csv++;
        }
    }

    while (*s)
    {
        if  ( *s == '"' )
        {
            r++;

            if  ( csv && r < n )
            {
                *csv = '"';
                csv++;
            }
        }

        r++;

        if  ( csv && r < n )
        {
            *csv = *s;
            csv++;
        }

        s++;
    }

    if  (q)
    {
        r++;

        if  ( csv && r < n )
        {
            *csv = '"';
            csv++;
        }
    }

    r++;
    if  ( csv ) *csv = '\0';

    return r;
}

size_t  col2unl(const char *col, char *unl, size_t n)
{
    return col2unl_r(col, unl, n, unl_delim);
}

size_t  col2unl_r(const char *col, char *unl, size_t n, char delimiter)
{
    size_t r = 0;
    const char *s = col;

    while (*s)
    {
        if  ( *s == delimiter || *s == ESC || *s == '\n' )
        {
            r++;

            if  ( unl && r < n )
            {
                *unl = ESC;
                unl++;
            }
        }

        r++;

        if  ( unl && r < n )
        {
            *unl = *s;
            unl++;
        }

        s++;
    }

    r++;
    if  ( unl ) *unl = '\0';

    return r;
}
