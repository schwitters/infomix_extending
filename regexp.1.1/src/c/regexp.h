/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */

#define NSUBEXP  10
typedef struct regexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char  regstart;		/* Internal use only. */
	char  reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int   regmlen;		/* Internal use only. */
	char  program[1];	/* Unwarranted chumminess with compiler. */
} regexp;

#define regerror(a)    oni_regerror(a)
#define regcomp(a)     oni_regcomp(a)
#define regexec(a,b)   oni_regexec(a,b)
#define regsub(a,b,c)  oni_regsub(a,b,c)
#define regsublen(a,b) oni_regsublen(a,b)

extern regexp *oni_regcomp(char *p);
extern int     oni_regexec(regexp *prog, char *string);
extern void    oni_regsub(regexp *prog, char *source, char *dest);

extern void    oni_regerror(char *s);

/*
 * Additions made by Chris Bosch.
 *
 */

extern int oni_regsublen(regexp *prog, char *source);

#define ALLOC_CHECK(allocation_statement) \
if ((allocation_statement) == NULL) \
	mi_db_error_raise((MI_CONNECTION *)NULL, MI_SQL, "VM001")

