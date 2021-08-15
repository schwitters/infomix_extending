/*
**  File: Random.h
**
** About:
**
**     Header files for Random stuff.
**
**     The functions in this code are taken from;
**
**     _Numerical_Recipes_In_C_
**
**/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
/*
** mi stuff
*/
#include <mi.h>
/*
** Stuff for Rand
*/
#define IA 16807
#define IM 2147483647
#define FM 2147483647.0
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876
#define PI 3.141592654

/*
** Structures
*/
typedef struct _bnldev_statics
{
    mi_integer    idum;
    mi_integer    nold;
    mi_real    pold, pc, plog, pclog, en, oldg;
} bnldev_statics;

typedef struct _gasdev 
{
    mi_integer    	iset;
    mi_real   gset;
    mi_integer    inum;
} gasdev_str;

/*
** Functions;
**
**   These are the private functions that are used within the 
**  BladeLet.
**
**/
mi_real my_rand ( mi_integer * );
mi_real gammln  ( mi_real );
mi_real bnldev  ( mi_real *, mi_integer, bnldev_statics * );








