/*
** Simple c function to be called from a DataBlade.
*/

#include <sys/types.h>
#include <stdlib.h>
#include <mi.h>

char *
call_c(input_value)
{
    int         arraylen = 5;
    int         *intarray;
    int         i;
    char        retbuf[60];

    /*
    ** Write to the log: We're here!
    */
    DPRINTF( "__myErrors__", 20, ("...in call_c()"));
    DPRINTF( "__myErrors__", 20, (".....calling malloc()"));

    intarray = (int *)malloc(arraylen);
    if (intarray == 0)
    { 
        return("Error from call_c!");
    }
    else
    {
        for (i = 0; i < arraylen; i++)
        {
            intarray[i] = i * input_value;
        }
    }

    strcpy(retbuf, "call_c: ");

    for (i=0; i< arraylen; i++)
    {
        sprintf(retbuf, "%s %d", retbuf, intarray[i]);
    }

    DPRINTF( "__myErrors__", 20, (".....calling free()"));
    free(intarray);

    DPRINTF( "__myErrors__", 20, (".....leaving call_c()"));

    return(retbuf);
}

