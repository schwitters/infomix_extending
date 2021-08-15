/* "@(#) $Id: unl_utils.h,v 1.1 2005/04/13 12:45:46 richard Exp $" */

/*
    'unl_utils.h'

    unl/csv file utilities.

    (C) Richard Harnden, 2005
    Please report bugs: <richard.harnden@lineone.net>

    This source is public domain.  Use/copy at will.  No warranties, etc.
*/

#ifndef UNL_UTILS_H
    #define UNL_UTILS_H

    #include <stdio.h>

    typedef char char_esc;

    int set_unl_delimiter(char delim);
    /*
        sets the global unl delimiter, returns 0 if delim is invalid.

        obviously this isn't thread-safe
    */

    typedef struct csv_buffer csv_buffer;
    csv_buffer *new_csv_buffer(void);
    void       delete_csv_buffer(csv_buffer *csvbuf);

    char_esc *fgets_csv(char_esc *s, size_t n, FILE *stream);
    char_esc *fgets_csv_r(char_esc *s,
                          size_t n,
                          FILE *stream,
                          csv_buffer *csvbuf);
    /*
        The trailing '\n' is not copied.

        fgets_csv is not thread-safe.

        Sets errno to:
            ENOMEM if 's' is too small
    */


    typedef struct unl_buffer unl_buffer;
    unl_buffer    *new_unl_buffer(char delimiter);
    void        delete_unl_buffer(unl_buffer *unlbuf);

    char_esc *fgets_unl(char_esc *s, size_t n, FILE *stream);
    char_esc *fgets_unl_r(char_esc *s, size_t n, FILE *stream, unl_buffer *unlbuf);
    /*
        The trailing '\n' is not copied.

        fgets_unl is not thread-safe.

        Sets errno to:
            ENOMEM if 's' is too small
    */


    int fputs_csv(const char_esc *s, FILE *stream);
    int puts_csv(const char_esc *s);
    /*
        as fputs, puts except s is formatted as csv
    */

    int fputs_unl(const char_esc *s, FILE *stream);
    int puts_unl(const char_esc *s);
    int fputs_unl_r(const char_esc *s, FILE *stream, char delimiter);
    int puts_unl_r(const char_esc *s, char delimiter);
    /*
        as fputs, puts except s is formatted as unl

        fputs_unl and puts_unl are not thread-safe.
    */


    int count_cols(const char_esc *s);
    /*
        returns the number of columns in s
    */

    size_t get_col(const char_esc *s, int colno, char *col, size_t n);
    /*
        colno is zero-based.

        col is un-escaped, ie contains the literal data.

        returns 0 and sets errno = ERANGE if colno is invalid, otherwise
        returns the number of characters required, even if this is >n.
    */

    size_t col2csv(const char *col, char *csv, size_t n);
    size_t col2unl(const char *col, char *unl, size_t n);
    size_t col2unl_r(const char *col, char *unl, size_t n, char delimiter);
    /*
        formats col as an unl or csv.

        col2unl is not thread-safe.

        returns the number of characters required, even if >n.
    */
#endif
