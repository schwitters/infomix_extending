/* ***************************************************************************
** vtidemo_ext.h
** ***************************************************************************
*/
typedef struct vtidemo_qual
{
    int     qual_isnull;           /* 1=true (return all rows), 0=false     */
    int     qual_issimple;         /* 1=simple predicate, 0=not             */
    /*
     * for simple predicates ie. f(column,const)
     */
    int     qual_negate;           /* negate result? 1=true                 */
    char    *qual_funcname;        /* name of function f()                  */
    int     qual_column;           /* column number in predicate            */
    int     qual_coltype;          /* column type. 1=integer, 2=char string */
    char    *qual_constant;        /* the constant, stored as char string.  */
 
    /*
     * for complex predicates ("and"/"or"):
     */
    int     qual_boolop;           /* 1=AND, 2=OR                           */
    int     qual_nquals;           /* number of quals in expr               */
    struct vtidemo_qual *qual_quals;   /* ptr to qual_nquals qd's           */
 
} vtidemo_qual_t;

/* outrec_t is used in the beginscan routine */
typedef  struct rec_out
{
    int    rowqualified;   /* 1=fully qualified, -1=not                 */
    int    rowid;          /* unique identifier for the row             */
    char   rowbuf[500];    /* row data                                  */
} outrec_t;

