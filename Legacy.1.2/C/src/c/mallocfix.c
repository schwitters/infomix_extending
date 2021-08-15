/*
 * Override the regular library verions of the malloc() routines.
 *
 * Should mimic the standard SYSV implementation except alloca().
 *
 * Author:	Cosmo, Informix Software, 16/04/1998
 *
 * Change History:    
 *
 *    Aug 11, 1998       wwwhite     Added DPRINTF statements for tracing
 *
 */

#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <mi.h>

#define malloc_func	mi_alloc
#define free_func	mi_free

#define ALIGN_TYPE	double

union memheader_u
{
    struct mh_s
    {
        size_t sz;
        void *ptr;
    } i;
    ALIGN_TYPE a;
};
#define mh_size i.sz
#define mh_ptr i.ptr

typedef union memheader_u memheader;

#define ALIGN(a,p)	(void *)((a-1+(unsigned long)(p))&(~(a-1)))

#ifndef min
#define min(a,b)	((a)<(b)?(a):(b))
#endif

void *
memalign(size_t align, size_t size)
{
    void *ret;
    void *new;
    memheader *mh;
    
    if (!(new = (void *) malloc_func (size + sizeof (memheader) + align)))
	return NULL;
	
    ret = ALIGN (align, (char *)new + sizeof (memheader));
    mh = (memheader *)ret - 1;
    mh->mh_size = size;
    mh->mh_ptr = new;
    
    return ret;
}
    
void *
malloc (size_t size)
{
    DPRINTF( "__myErrors__", 20, (".......in local malloc() routine"));
    return (memalign (sizeof (ALIGN_TYPE), size));
}

    
void
free (void *ptr)
{
    DPRINTF( "__myErrors__", 20, (".......in local free() routine"));
    free_func ((void *)(((memheader *)ptr - 1)->mh_ptr));
}


void *
realloc(void *ptr, size_t newsize)
{
    void *newptr = NULL;
    memheader *mh;
    size_t oldsize = 0;

    DPRINTF( "__myErrors__", 20, (".......in local realloc() routine"));
    if (ptr)
    {
	mh = (memheader *)ptr - 1;
	oldsize = mh->mh_size;
    }

    if (newsize > oldsize)
    {
	newptr = malloc (newsize);

	if (ptr && newptr)
	    memcpy (newptr, ptr, min (oldsize, newsize));
	
	if (ptr)
	    free (ptr);
    }
    else
    {
	if (newptr = ptr)
	    mh->mh_size = newsize;
    }

    return newptr;
}


void *
calloc(size_t nelem, size_t elsize)
{
    size_t fullsz = nelem * elsize;
    void * ret = malloc (fullsz);
    
    DPRINTF( "__myErrors__", 20, (".......in local calloc() routine"));
    if (ret != NULL)
	memset (ret, '\0', fullsz);
    
    return ret;
}


void *
valloc(size_t size)
{
    static size_t sc_pagesize = 0;
    void *ptr;

    DPRINTF( "__myErrors__", 20, (".......in local valloc() routine"));
    if (!sc_pagesize)
	sc_pagesize = sysconf(_SC_PAGESIZE);
    
    ptr = memalign (sc_pagesize, size);

    return (ptr);
}


