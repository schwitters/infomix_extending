//
// Simple C++ function to be called from a DataBlade.
//

#include <sys/types.h>
#include <stdlib.h>
#include <mi.h>

class integer
{
    private:

        int number;

    public:

       integer();
      ~integer();
        void store(int);
        int get();
};

int integer::get()
{
    return number;
}

integer::~integer()
{
number = 17;
}


integer::integer()
{
    number = -1;
}

void integer::store(int n)
{
    number = n;
}

#ifdef __cplusplus
extern "C"
{
#endif

char *
call_cplusplus(int input_value)
{
    int         arraylen = 5;
    integer     *intarray;
    int         i;
    char        retbuf[60];

    char        *strcpy(char *dst, const char *src);
    int         sprintf(char *s, const char *format /* args */ ...);

    /*
    ** Write to the log: We're here!
    */
    DPRINTF( "__myErrors__", 20, ("...in call_cplusplus()"));

    DPRINTF( "__myErrors__", 20, (".....calling the new[] operator"));
    intarray = new integer[arraylen];
    if (intarray == 0)
    { 
        return("Error from call_cplusplus!");
    }
    else
    {
        for (int i = 0; i < arraylen; i++)
        {
            intarray[i].store(i * input_value);
        }
    }

    strcpy(retbuf, "call_cplusplus: ");

    for (i=0; i< arraylen; i++)
    {
        sprintf(retbuf, "%s %d", retbuf, intarray[i].get());
    }

    DPRINTF( "__myErrors__", 20, (".....calling the delete[] operator"));
    delete [] intarray;

    DPRINTF( "__myErrors__", 20, (".....leaving call_cplusplus()"));
    return(retbuf);
}

#ifdef __cplusplus
}
#endif

