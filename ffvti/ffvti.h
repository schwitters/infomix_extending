/*
 * include file for ffvti.c
 *
 * Written by Jacques Roy
 * Last modification 4/17/2001
 */

/* stdio FILE structure, used to buffer file reads */
#define BUFSIZE  1024

typedef struct
{
        mi_integer      _cnt;   /* number of available characters in buffer */
        mi_string       *_ptr;  /* next character from/to here in buffer */
        mi_string       *_base; /* the buffer */
        mi_char         _flag;  /* the state of the stream */
        mi_integer      _file;  /* UNIX System file descriptor */
} FF_FILE;

typedef struct ff_info {
  FF_FILE       *fd[1];
  MI_CONNECTION *pconn;
  MI_ROW        *prow;
  MI_DATUM      *pdatum;
  mi_boolean    *pbool;
  mi_char       delim;
} FF_INFO;

typedef struct ff_type_val {
	mi_string *type;
	mi_integer val;
} FF_TYPE_VAL;

mi_integer ff_open(MI_AM_TABLE_DESC *ptbldesc);
mi_integer ff_beginscan(MI_AM_SCAN_DESC *pscandesc);
mi_integer ff_getnext(MI_AM_SCAN_DESC *pscandesc,
                      MI_ROW **retrow, mi_integer *retrowid);
mi_integer ff_endscan(MI_AM_SCAN_DESC *pscandesc);
mi_integer ff_close(MI_AM_TABLE_DESC *ptbldesc);

mi_integer ff_open_file(MI_AM_TABLE_DESC *ptbldesc, FF_INFO *pff_info);
mi_string *ff_get_param(mi_string *pamparam, mi_string *pname);
mi_integer ff_log_qual(FF_INFO *pff_info, MI_AM_TABLE_DESC *ptbldesc, 
						MI_AM_QUAL_DESC *pqualdesc);
mi_integer ff_set_col(FF_INFO *pff_info, MI_TYPE_DESC *ptype,
						mi_integer i, mi_string *buf);

FF_FILE *ff_fopen(mi_string *ppath, mi_char mode);
mi_integer ff_fclose(FF_FILE *fd);
mi_integer ff_rewind(FF_FILE *fd);
mi_string *ff_fgets(mi_string *s, mi_integer n, FF_FILE *stream);
mi_integer ff_fputs(const char *s, FF_FILE *stream);
