/*
**  File:  Random.c
**
** About:
**  
**         These are the functions that are used, and exported,
**      by the Random Blade. 
**
**         A little background. Computer generated random numbers are
**      always 'pseudo-random', and they are usually generated from
**      some variety of a loop using loop over some seed value. This
**      means that I need to use the per_function cache to contain 
**      the changing seed.
**
**        Second, the simple random distribution really isn't enough 
**      for all purposes. Therefore, I have included three types of
**      distribution: random, binomial, and gaussian (or normal). 
**      There are several others -- notably Poisson and Zipf --
**      but I've not included them here. 
**
**       For the pedants among you, the gaussian and random variables
**      are continous while the binomial is discrete.
**
**       The idea is that you use these functions in SQL statements,
**      and for the duration of the SQL statement you will get a series
**      of random variables that comply with the distribution function.
**      This is most useful for data generation. For example, we ktion.
**      This is most useful for generating test data, and you can 
**      compute skewed or uniform distributions of any variance and range.
**
**   Press, William H., Teukolsky, Saul A., Vetterling, William T.,
**   and Flannery, Brian P. _Numerical_Recipes_in_C_:_The_Art_of_
**   Scientific_Computing_Second_Edition_ Cambridge University Press,
**   New York. 1992.
**
**           Copies of all of these can be found at;
**
**     <A HREF>http://cfatab.harvard.edu/nr/bookcpdf.html<\HREF>
**                                                                          
*/
#include "Random.h"
/*****************************************************************************
**
**   Function: RandomWithSeed
**
**  This function takes a seed value in the query statement. This is
**  necessary on occasions because if a query statement contains two 
**  Random functions,  it is highly likely -- indeed it is almost certain --
**  than the 'time' seed for both of these functions will be identical, and
**  will therefore generate identical sequences of values.
**
**/
mi_real * RandomWithSeed ( 
mi_integer nArg1, 
MI_FPARAM * pFParam )
{
    mi_integer * pnSeed;
    mi_real    * rRetVal;
    
    if ((pnSeed = mi_fp_funcstate(pFParam)) == NULL)
    {
        /*
        ** If this is the first pass, then I need to establish the scratch
		** memory.
        */
        pnSeed = (mi_integer *) mi_dalloc(sizeof(mi_integer),
                                          PER_COMMAND);
        *pnSeed = nArg1;
        mi_fp_setfuncstate(pFParam, pnSeed);
    }

    rRetVal  = (mi_real *)mi_alloc(sizeof(mi_real)); /*NOTE: sizeof(real) > 4 */
    *rRetVal = my_rand(pnSeed);

    return (rRetVal);
}
/*****************************************************************************
**
**   Function: RandomWithSeed
**
**  This function takes a seed value in the query statement. This is
**  necessary on occasions because if a query statement contains two
**  Random functions,  it is highly likely -- indeed it is almost certain --
**  than the 'time' seed for both of these functions will be identical, and
**  will therefore generate identical sequences of values.
**
**/
mi_integer
RandomWithSeedMax (
mi_integer nSeed,
mi_integer nMax,
MI_FPARAM * pFParam )
{
    mi_integer * pnSeed;
    mi_real    rRetVal;

    if ((pnSeed = mi_fp_funcstate(pFParam)) == NULL)
    {
        pnSeed = (mi_integer *) mi_dalloc(sizeof(mi_integer),
                                          PER_COMMAND);
        *pnSeed = nSeed;
        mi_fp_setfuncstate(pFParam, pnSeed);
    }

    rRetVal = my_rand(pnSeed);
    return (rRetVal * nMax);
}
/*****************************************************************************
**
**   Function: BinomialwithSeed
**
**    This function is similar to RandomWithSeed in that it's designed
**   for circumstances where there is more than one mention of the
**   function in a single query.
**
**/
mi_real * BinomialWithSeed ( 
mi_integer  nTrials,
mi_real   * prProb,
mi_integer  nrseed, 
MI_FPARAM * pFParam )
{
    mi_real        * rRetVal;
    bnldev_statics * pbnl_statics;

    if ((pbnl_statics = mi_fp_funcstate(pFParam)) == NULL)
    {
        pbnl_statics = (bnldev_statics *)mi_dalloc(sizeof(bnldev_statics),
                                                   PER_COMMAND);
		pbnl_statics->idum = nrseed;
		pbnl_statics->nold = nTrials - 1;
		pbnl_statics->pold = *prProb - 1.0;

        mi_fp_setfuncstate(pFParam, pbnl_statics);
    }

    rRetVal  = (mi_real *)mi_alloc(sizeof(mi_real));

	*rRetVal = bnldev ( prProb, nTrials, pbnl_statics);

	return (rRetVal);

}
/*****************************************************************************
**
**  Function: NormalWithSeed
**
**  This function returns a normal or gaussian distribution with a mean of
** zero and unit variance.
**
*/
mi_real * 
NormalWithSeed
(
mi_integer  nSeed,
MI_FPARAM * pFparam
)
{
    gasdev_str * pGds;
    mi_real fac,rsq,v1,v2;
	mi_real * pflRetVal;
   
    if ((pGds = (gasdev_str * )mi_fp_funcstate(pFparam)) == NULL)
    {
        pGds = (gasdev_str *)mi_dalloc(sizeof(gasdev_str), PER_COMMAND);
		pGds->inum = nSeed;
        mi_fp_setfuncstate(pFparam,(char *)pGds);
    }

	pflRetVal = mi_alloc(sizeof(mi_real));
 
	if ( nSeed < 0 ) pGds->iset = 0;
    if(pGds->iset == 0)
    {
        do {
            v1=2.0*my_rand(&(pGds->inum))-1.0;
            v2=2.0*my_rand(&(pGds->inum))-1.0;
            rsq=v1*v1+v2*v2;
        } while (rsq >= 1.0 || rsq == 0.0);
        fac=sqrt(-2.0*log(rsq)/rsq);
        pGds->gset=v1*fac;
        pGds->iset=1;
        *pflRetVal =  v2*fac;
    } else
    {
        pGds->iset=0;
        *pflRetVal = pGds->gset;
    }

	return pflRetVal;
}
/*****************************************************************************
**
**  Function: NormalNoSeed
**
**  This function returns a normal or gaussian distribution with a mean of
** zero and unit variance.
**/
mi_real *
NormalNoSeed
(
MI_FPARAM * pFparam
)
{
	mi_integer nSeed;
    gasdev_str * pGds;

    if ((pGds = (gasdev_str * )mi_fp_funcstate(pFparam)) == NULL)
    {
        pGds = (gasdev_str *)mi_dalloc(sizeof(gasdev_str), PER_COMMAND);
        pGds->inum = (mi_integer)time((time_t) NULL);
        mi_fp_setfuncstate(pFparam,(char *)pGds);
    }
	return NormalWithSeed(pGds->inum, pFparam);
}

/*****************************************************************************
**
**  Function: NormalMeanDev
**
**  This function returns a normal or gaussian distribution with a mean of
** argument 1 and a deviation of Argument 2.
**
**/
mi_real *
NormalMeanDev
(
mi_real   * prMean,
mi_real   * prDev,
MI_FPARAM * pFparam
)
{
	mi_real    * prRetVal;
    gasdev_str * pGds;

    if ((pGds = (gasdev_str * )mi_fp_funcstate(pFparam)) == NULL)
    {
        pGds = (gasdev_str *)mi_dalloc(sizeof(gasdev_str), PER_COMMAND);
        pGds->inum = (mi_integer)time((time_t) NULL);
        mi_fp_setfuncstate(pFparam,(char *)pGds);
    }
	prRetVal =  NormalWithSeed(pGds->inum, pFparam);

	*prRetVal = *prMean + ((*prDev) * (*prRetVal));

	return prRetVal;
}
/*****************************************************************************
**
**  Function: NormalMeanDevSeed
**
**  This function returns a normal or gaussian distribution with a mean of
** argument 1 and a deviation of Argument 2. It uses as a seed value the
** third argument.
**
**/
mi_real *
NormalMeanDevSeed
(
mi_real   * prMean,
mi_real   * prDev,
mi_integer  nSeed,
MI_FPARAM * pFparam
)
{
    mi_real    * prRetVal;
    gasdev_str * pGds;
 
    if ((pGds = (gasdev_str * )mi_fp_funcstate(pFparam)) == NULL)
    {
        pGds = (gasdev_str *)mi_dalloc(sizeof(gasdev_str), PER_COMMAND);
        pGds->inum = nSeed;
        mi_fp_setfuncstate(pFparam,(char *)pGds);
    	prRetVal =  NormalWithSeed(pGds->inum, pFparam); /* initialize */
    }
    prRetVal =  NormalWithSeed(pGds->inum, pFparam);
 
    *prRetVal = *prMean + ((*prDev) * (*prRetVal));
 
    return prRetVal;
}

/*****************************************************************************
**
**   Function: RandomNoSeed
**
**     This function simply returns a uniform random distribution based
**   on the time. Unfortunately, two queries running concurrently will
**   yield the same series of results.
**
**/
mi_real * RandomNoSeed (
MI_FPARAM * pFParam
)
{
    mi_integer	* pnSeed;

    if ((pnSeed = mi_fp_funcstate(pFParam)) == NULL)
    {
        pnSeed = (mi_integer *) mi_dalloc(sizeof(mi_integer),
                                         PER_COMMAND);
        *pnSeed = (mi_integer)time((time_t) NULL);
        mi_fp_setfuncstate(pFParam, pnSeed);
    }

    return RandomWithSeed ( *pnSeed, pFParam );
}
/*****************************************************************************
**
**  Function:  RandomWithMax
**
**    This function returns a uniform random distribution within the
**   range of zero to one less than the value of the argument passed.
**
**/
mi_integer
RandomWithMax (
mi_integer	nMax,
MI_FPARAM * pFParam
)
{
    mi_integer  * pnSeed;
	mi_real     * prInternalResult;
 
    if ((pnSeed = mi_fp_funcstate(pFParam)) == NULL)
    {
        pnSeed = (mi_integer *) mi_dalloc(sizeof(mi_integer),
                                         PER_COMMAND);
        *pnSeed = (mi_integer)time((time_t) NULL);
        mi_fp_setfuncstate(pFParam, pnSeed);
    }

	prInternalResult = RandomWithSeed(*pnSeed, pFParam );

	return ((*prInternalResult) * nMax);
}

/*****************************************************************************
**
**   Function: BinomialNoSeed
**
**     This function simply returns a binomial random distribution based
**   on the time. Unfortunately, two queries running concurrently will
**   yield the same series of results.
**
**/
mi_real * BinomialNoSeed (
mi_integer  nTrials,
mi_real   * prProb,
MI_FPARAM * pFParam
)
{
    bnldev_statics * pbnl_statics;

    if ((pbnl_statics = mi_fp_funcstate(pFParam)) == NULL)
    {
        pbnl_statics = (bnldev_statics *)mi_dalloc(sizeof(bnldev_statics),
                                                   PER_COMMAND);
	pbnl_statics->idum = (mi_integer)time(NULL);
	pbnl_statics->nold = nTrials - 1;
	pbnl_statics->pold = *prProb - 1.0;

        mi_fp_setfuncstate(pFParam, pbnl_statics);
    }

    return BinomialWithSeed ( nTrials, prProb, pbnl_statics->idum, pFParam );
}

/*
*****************************************************************************
**
**   Private Functions
**
**    These functions are based on the material in 'Numerical Algorithms
**  in C'.
**
**/
mi_real my_rand ( 
mi_integer * pLseed )
{
    mi_integer  k;
    mi_real ans;
    void  * pFArg;

    *pLseed ^= MASK;
    k=(*pLseed)/IQ;
    *pLseed=IA*(*pLseed-k*IQ)-IR*k;
    if (*pLseed < 0) *pLseed += IM;
    ans=AM*(*pLseed);
    *pLseed ^= MASK;
    return ans;

}

mi_real gammln ( mi_real xx )
{
    double    x,y,tmp,ser;
    double cof[6] = {76.18009172947146,
             -86.50532032941677,
             24.01409824083091,
             -1.231739572450155,
             0.1208650973866179e-2,
             -0.5395239384953e-5};
    mi_integer    j;

    y=x=xx;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    for (j=0;j<=5;j++)
        ser += cof[j]/++y;

    return -tmp+log(2.5066282746310005*ser/x);
}

mi_real bnldev ( 
mi_real * pp, 
mi_integer n, 
bnldev_statics * b_stat )
{
    mi_integer    j;
    mi_real       am, em, g, angle, p, bnl, sq, t, y;
    mi_integer    * idum;

    idum = &(b_stat->idum);

    p=(*pp <= 0.5 ? *pp : 1.0 - *pp);

    am=n*p;

    if (n < 25 )
    {
        bnl=0.0;
        for(j=1;j<=n;j++)
            if (my_rand(idum) < p) ++bnl;
    } else if (am < 1.0 ) {
        g = exp(-am);
        t=1.0;
        for(j=0;j<=n;j++)
        {
            t *= my_rand(idum);
            if (t < g) break;
        }
        bnl=(j <= n ? j : n);
    } else {
        if (n != (b_stat->nold))
        {
            b_stat->en=n;
            b_stat->oldg=gammln(b_stat->en + 1.0);
            b_stat->nold=n;
        }
        if ( p != b_stat->pold)
        {
            b_stat->pc=1.0-p;
            b_stat->plog=log(p);
            b_stat->pclog=log(b_stat->pc);
            b_stat->pold=p;
        }

        sq=sqrt(2.0*am*b_stat->pc);

        do
        {
            do
            {
                angle=PI*my_rand(idum);
                y=tan(angle);
                em=sq*y+am;
            } while (em < 0.0 || em >= (b_stat->en + 1.0));
            em=floor(em);
            t=1.2*sq*(1.0+y*y)*exp(b_stat->oldg-gammln(em+1.0)
              -gammln(b_stat->en-em+1.0)+em*b_stat->plog+
              (b_stat->en-em)*b_stat->pclog);
        } while (my_rand(idum) > t);
        bnl=em;
    }
    if ( p != *pp) bnl=n-bnl;

    return bnl;
}
