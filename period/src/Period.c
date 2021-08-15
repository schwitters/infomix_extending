/*
** Title:               Period.c
** SCCSid:              %W% %E% %U%
** CCid:                %W% %E% %U%
** Author:              
**
**  The following is placed here to insure
**  that name "mangling" does not occur.
*/
#ifdef __cplusplus

extern "C"
{

#endif

/* Standard library includes. */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>

/* Used by Informix GLS routines. */
#include <ifxgls.h>


/* Include when accessing the Informix API. */
#include <mi.h>

/* This is the project include file. */
#include "Period.h"

/*****************************************************************************
**
**   Internal support routines called by other, public routines.
**
******************************************************************************
*/
/***********************************************************************
**
**  Function:
**
**            new_Period()
**
**  About:
**
**            Returns a new, unpopulated instance of the Period data
**            structure. Used throughout the code wherever a new
**            instance of the Period object is required.
*/
Period * 
new_Period ()
{
	Period * pPeriod;
	MI_CONNECTION * Gen_Con;
    if ((pPeriod = (Period *)mi_alloc(sizeof(Period))) == NULL)
    {
        /*
        ** Memory allocation has failed so issue
        ** the following message and quit.
        **
        **     "Memory allocation has failed in new_Period."
        */
		Gen_Con = mi_open ( NULL, NULL, NULL );
        DBDK_TRACE_ERROR( "new_Period", ERRORMESG2, 10 );
        /* not reached */
    }
	return pPeriod;
}
/***********************************************************************
**
**  Function:
**
**            copy_Period()
**
**  About:
**
**            Returns a new, unpopulated instance of the Period data
**            structure. Used throughout the code wherever a new
**            instance of the Period object is required.
*/
Period * 
copy_Period ( Period * pPeriod)
{
	Period	*	pRetPeriod;

	pRetPeriod = new_Period();
	memcpy (pRetPeriod, pPeriod, sizeof(Period));

	return pRetPeriod;
}
/*****************************************************************************
**
**     Function: PeriodDateCMP
**
**  Description:  
**
**               This function is not strictly necessary, because the way
**               that EPOCH and FOREVER are handled in the Period fits
**               well into the specification of the DATE data value. However,
**               seperating the compare is consistent with the design of the 
**               range pattern in general. Ranges involving other types
**               will need to overload this 'C' function.
*/  
mi_integer 
PeriodDateCMP ( mi_date * DateOne, mi_date * DateTwo)
{
	mi_integer nRetVal = 0;

	if ( *DateOne > *DateTwo ) 
		nRetVal = 1;
	else if ( *DateOne < *DateTwo )
		nRetVal = -1;

	return nRetVal;
}
/****************************************************************************
**
**     Function:
**
**        Check_Period
**
**  Description:
**
**        This function checks the correctness of the Period object. It
**        checks to see whether the finish of the Period is before the
**        start.
*/
void 
Check_Period ( Period * pPeriod )
{
	if ( pPeriod->finish < pPeriod->start )
	{
		mi_db_error_raise( NULL, MI_EXCEPTION, 
						   "Check Failed: start must preceed finish");
		/* not reached */
	}
}
/****************************************************************************
**
**    Function(s):  
**
**              Period_Epoch() and Period_Forever()
**
**    About:
**
**              Support functions for open starts and finishes for the
**              Period instances.
*/
mi_date
Period_Epoch ()
{
        return INT_MIN;
}

mi_date
Period_Forever()
{
        return INT_MAX;
}
/****************************************************************************
**
**     Function:
**
**        Period_Start_is_Epoch
**
**  Description:
**
**      Boolean check to determine whether or not the start of the Period
**     is open. The design of the Period type calls for support of an 
**     open start and finish.
*/
mi_boolean 
Period_Start_is_Epoch ( Period    * pPeriod,
						MI_FPARAM * Gen_fparam )
{
	if (pPeriod->start == Period_Epoch()) 
		return MI_TRUE;

	return MI_FALSE;
}
/****************************************************************************
**
**     Function:
**
**        Period_Set_Start_Epoch
**
**  Description:
**
**       Function that sets the start of a Period to Open.
**
*/
void 
Set_Period_Start_Epoch_internal ( Period    * pPeriod,
						          MI_FPARAM * Gen_fparam  )
{
	pPeriod->start = Period_Epoch();
}

Period * 
Period_Set_Start_Epoch ( Period    * pPeriod,
						 MI_FPARAM * Gen_fparam  )
{
	Period	*	pRetPeriod;

	/* Remember: Do not modify Arg structure. Allocate memory for the UDT */
	/* copy data there, and modify it there.                              */
	pRetPeriod = copy_Period( pPeriod );
	Set_Period_Start_Epoch_internal ( pRetPeriod, Gen_fparam );

	Check_Period( pRetPeriod );

	return pRetPeriod;
}
/****************************************************************************
**
**     Function:
**
**        Period_Finish_is_Unbound
**
**  Description:
**
**      Boolean check to determine whether or not the finish of the Period
**     is open.
**
*/
mi_boolean 
Period_Finish_is_Unbound ( Period    * pPeriod,
					       MI_FPARAM * Gen_fparam  )
{
	if ( pPeriod->finish == Period_Forever() )
		return MI_TRUE;

	return MI_FALSE;
}
/****************************************************************************
**
**     Function:
**
**        Period_Set_Finish_Unbound
**
**  Description:
**
**        Function that sets the finish of a Period to Open.
*/
void 
Set_Period_Finish_Unbound_internal ( Period    * pPeriod,
						             MI_FPARAM * Gen_fparam )
{
	pPeriod->finish = Period_Forever();
}

Period * 
Period_Set_Finish_Unbound ( Period    * pPeriod,
					        MI_FPARAM * Gen_fparam )
{
	Period	*	pRetPeriod;
	/* Remember: Do not modify Arg structure. Allocate memory for the UDT */
	/* copy data there, and modify it there.                              */
	pRetPeriod = copy_Period( pPeriod );
	Set_Period_Finish_Unbound_internal ( pRetPeriod, Gen_fparam );

	Check_Period( pRetPeriod );

	return pRetPeriod;
}
/*******************************************************************************
**
** Function name:
**
**	Period_X_Period
**
** Description:
**
**   This is the guts of the Period magic. This function examines the 
**  state of the two Periods and returns an mi_integer that reflects
**  the relationship between them. This is called by all of the other
**  functions to determine what the relationship between the two args
**  is.
**
*/
mi_integer
Period_X_Period ( Period     * pPeriodOne,
			      Period     * pPeriodTwo )
{
	mi_integer	nRetVal = 0;
	mi_integer	S1S2, S1E2, E1S2, E1E2;
	mi_integer  arRetVals[3][3][3][3] = RANGE_X_RET_VALS;

	S1S2 = PeriodDateCMP(&(pPeriodOne->start), &(pPeriodTwo->start));
	S1E2 = PeriodDateCMP(&(pPeriodOne->start), &(pPeriodTwo->finish));
	E1S2 = PeriodDateCMP(&(pPeriodOne->finish),&(pPeriodTwo->start));
	E1E2 = PeriodDateCMP(&(pPeriodOne->finish),&(pPeriodTwo->finish));

	if ((nRetVal = arRetVals[S1S2+1][S1E2+1][E1S2+1][E1E2+1]) == P_CMP_ERROR )
	{
		/*
		** In this case, I am going to by-pass the normal error handling
		** facilities and raise this one directly. This error should
		** never occur, and this code is here as a defensive programming
		** measure.
		*/
		mi_string * pchErrCond;
		pchErrCond = (mi_string *)mi_alloc(64);
		sprintf( pchErrCond, 
                 "Compare Error: arRetVals[%d][%d][%d][%d] is zero",
                 S1S2+1, S1E2+1, E1S2+1, E1E2+1 );
		mi_db_error_raise ( NULL, MI_EXCEPTION, pchErrCond);
		/* not reached */
	}

	return nRetVal;
}
/****************************************************************************
**
**   Function name:
** 
**             Period_X_to_String
**
**   About:
**
**      This is useful for diagnostic purposes. I would recommend turning it
**   off in a production system.
**/
mi_lvarchar *
Period_X_to_String ( Period     * pPeriodOne,
					 Period     * pPeriodTwo,
				     MI_FPARAM  * Gen_fparam )
{
	mi_char	*	pchOutput;
	mi_integer  nRetVal;

	switch ( nRetVal = Period_X_Period ( pPeriodOne, pPeriodTwo ) )
	{
		case EQ_EQ_EQ_EQ: /* 1 ( 1 -> 1 )   ( 1 -> 1 )  EQ  */
			pchOutput = "EQ_EQ_EQ_EQ";
		 break;
		case EQ_LT_EQ_LT: /* 2  ( 1 -> 1 )   ( 1 -> 2 )  LT  */
			pchOutput = "EQ_LT_EQ_LT";
		 break;
		case LT_LT_LT_LT: /* 3   ( 1 -> 1 )   ( 2 -> 2 )  LT  */
			pchOutput = "LT_LT_LT_LT";
		 break;
		case EQ_EQ_GT_GT: /* 4   ( 1 -> 2 )   ( 1 -> 1 )  GT  */
			pchOutput = "EQ_EQ_GT_GT";
		 break;
		case EQ_LT_GT_EQ: /* 5   ( 1 -> 2 )   ( 1 -> 2 )  EQ  */
			pchOutput = "EQ_LT_GT_EQ";
		 break;
		case EQ_LT_GT_LT: /* 6   ( 1 -> 2 )   ( 1 -> 3 )  LT  */
			pchOutput = "EQ_LT_GT_LT";
		 break;
		case LT_LT_EQ_EQ: /* 7   ( 1 -> 2 )   ( 2 -> 2 )  LT  */
			pchOutput = "LT_LT_EQ_EQ";
		 break;
		case LT_LT_EQ_LT: /* 8   ( 1 -> 2 )   ( 2 -> 3 )  LT  */
			pchOutput = "LT_LT_EQ_LT";
		 break;
		case EQ_LT_GT_GT: /* 9   ( 1 -> 3 )   ( 1 -> 2 )  GT  */
			pchOutput = "EQ_LT_GT_GT";
		 break;
		case LT_LT_GT_GT: /* 10  ( 1 -> 3 )   ( 2 -> 2 )  LT  */
			pchOutput = "LT_LT_GT_GT";
		 break;
		case LT_LT_GT_EQ: /* 11  ( 1 -> 3 )   ( 2 -> 3 )  LT  */
			pchOutput = "LT_LT_GT_EQ";
		 break;
		case LT_LT_GT_LT: /* 12  ( 1 -> 3 )   ( 2 -> 4 )  LT  */
			pchOutput = "LT_LT_GT_LT";
		 break;
		case GT_GT_GT_GT: /* 13  ( 2 -> 2 )   ( 1 -> 1 )  GT  */
			pchOutput = "GT_GT_GT_GT";
		 break;
		case GT_EQ_GT_EQ: /* 14  ( 2 -> 2 )   ( 1 -> 2 )  GT  */
			pchOutput = "GT_EQ_GT_EQ";
		 break;
		case GT_LT_GT_LT: /* 15  ( 2 -> 2 )   ( 1 -> 3 )  GT  */
			pchOutput = "GT_LT_GT_LT";
		 break;
		case GT_EQ_GT_GT: /* 16  ( 2 -> 3 )   ( 1 -> 2 )  GT  */
			pchOutput = "GT_EQ_GT_GT";
		 break;
		case GT_LT_GT_EQ: /* 17  ( 2 -> 3 )   ( 1 -> 3 )  GT  */
			pchOutput = "GT_LT_GT_EQ";
		 break;
		case GT_LT_GT_GT: /* 18  ( 2 -> 4 )   ( 1 -> 3 )  GT  */     
			pchOutput = "GT_LT_GT_GT";
		 break;
		default:
			pchOutput = mi_alloc(32);
			sprintf( pchOutput, "Invalid: %d", nRetVal );
		 break;
	}
	return mi_string_to_lvarchar( pchOutput );
}
/*******************************************************************************
**
**      Function:     PeriodAfterNotTouches
**
**  Description: 
**
**     Returns MI_TRUE iff. the first is completely after the second.
**
**     Returns TRUE iff.the start of the first is greater than the
**   finish of the first.
*/
mi_boolean
PeriodAfterNotTouches ( Period * pPeriodOne,
				           Period * pPeriodTwo,
				           MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo );
	
	switch ( nXVal )
	{
		case GT_GT_GT_GT:
			bRetVal = MI_TRUE;
		 break;
	}

	return bRetVal;
}
/*******************************************************************************
**
**      Function:     PeriodAfterTouches
**
**  Description: 
**
**     Returns MI_TRUE iff. the first is after the second.
**
**     Returns TRUE iff.the start of the first is equal to the 
**   end of the second.
**
*/
mi_boolean
PeriodAfterTouches ( Period * pPeriodOne,
				        Period * pPeriodTwo,
				        MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo );
	switch ( nXVal )
	{
		case EQ_EQ_EQ_EQ:
		case EQ_EQ_GT_GT:
		case GT_EQ_GT_EQ:
		case GT_EQ_GT_GT:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;
}
/*******************************************************************************
**
**      Function:     PeriodBeforeNotTouches
**
**  Description: 
**
**     Returns MI_TRUE iff. the first period is completely before
**   the second.
**
**      That is, returns MI_TRUE iff. the finish of the first is 
**    LT the start of the second.
**
*/
mi_boolean
PeriodBeforeNotTouches ( Period * pPeriodOne,
				            Period * pPeriodTwo,
				            MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo );
	switch ( nXVal )
	{
		case LT_LT_LT_LT:
			bRetVal = MI_TRUE;
		 break;
	}

	return bRetVal;
}
/*******************************************************************************
**
**      Function:     PeriodBeforeTouches
**
**  Description: 
**
**     Returns MI_TRUE iff. the first period is before
**   the second and touches it.
**
**      That is, returns MI_TRUE iff. the finish of the first is 
**    EQ the start of the second. 
**
*/
mi_boolean
PeriodBeforeTouches ( Period * pPeriodOne,
				         Period * pPeriodTwo,
				         MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo );
	switch ( nXVal )
	{
		case EQ_EQ_EQ_EQ:
		case EQ_LT_EQ_LT:
		case LT_LT_EQ_EQ:
		case LT_LT_EQ_LT:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;

}
/**********************************************************************
**
**      Function:  PeriodCompare
**
**   Description:
**  
**    This function is only ever used in circumstances where the engine
**   needs to eliminate duplicates from a set, or do an equi-join. The
**   key thing is that it needs to be consistent.
**
**    So, the model is than EQ_EQ_EQ_EQ and EQ_LT_GT_EQ are equal.
**    
*/
mi_integer
PeriodCompare ( Period    * pPeriodOne,
				Period    * pPeriodTwo,
				MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal, nRetVal;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{

		case EQ_EQ_EQ_EQ:
		case EQ_LT_GT_EQ:
			nRetVal = 0;
		break;
		
		case EQ_LT_EQ_LT:
		case LT_LT_LT_LT:
		case EQ_LT_GT_LT:
		case LT_LT_EQ_EQ:
		case LT_LT_EQ_LT:
		case LT_LT_GT_GT:
		case LT_LT_GT_EQ:
		case LT_LT_GT_LT:
			nRetVal = -1;
		 break;

		default:
			nRetVal = 1;
		 break;
	}
	return nRetVal;
}
/**********************************************************************
**
**      Function:  PeriodEqual
**
**   Description:
**  
**      This is conistent with the PeriodCompare.
**    
*/
mi_boolean
PeriodEqual ( Period    * pPeriodOne,
			  Period    * pPeriodTwo,
			  MI_FPARAM * Gen_fparam )
{
	mi_integer	nIntCmp;
	mi_boolean	bRetVal = MI_FALSE;

	if ((nIntCmp = PeriodCompare (pPeriodOne, 
		                          pPeriodTwo, 
								  Gen_fparam)) == 0 ) 
		bRetVal = MI_TRUE;

	return bRetVal;
}
/**********************************************************************
**
**      Function:  PeriodNotEqual
**
**   Description:
**  
**     This is consistent with  PeriodCompare.
**    
*/
mi_boolean
PeriodNotEqual ( Period    * pPeriodOne,
			     Period    * pPeriodTwo,
				 MI_FPARAM * Gen_fparam )
{
	mi_integer	nIntCmp;
	mi_boolean	bRetVal = MI_TRUE;

	if ((nIntCmp = PeriodCompare (pPeriodOne, 
		                          pPeriodTwo, 
								  Gen_fparam)) == 0 ) 
		bRetVal = MI_FALSE;

	return bRetVal;
}
/***********************************************************************
**
**  SPATIAL OPERATOR CLASS UDF IMPLEMENTATION 
**
**
**   OK. I am going to try an experiment with the Operator Class
**  functionality. Instead of just building the spatial operators
**  I am going to build all 7 of the Temporal relations 
**  listed in Snodgrass, and slap-em-all in an operator class.
**  This ought to work, because all of them ought to be OK with
**  the R-Tree scans.
**
*/
/***********************************************************************
**
**      Function:   PeriodContainsTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first Period Contains the second.
**
**     This implements the "classic" contains, where the start or
**  the finish of the Period can "touch" the start or finish of
**  the enclosing Period.
**
*/
mi_boolean
PeriodContainsTouches ( Period    * pPeriodOne,
			     	    Period    * pPeriodTwo,
				        MI_FPARAM * Gen_fparam  )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case EQ_EQ_EQ_EQ:
		case EQ_EQ_GT_GT:
		case EQ_LT_GT_EQ:
		case LT_LT_EQ_EQ:
		case EQ_LT_GT_GT:
		case LT_LT_GT_GT:
		case LT_LT_GT_EQ:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodContainsNotTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first Period Contains the second
**  completely. That is, the start of second is GT start of first, and
**  end of first is LT end of second.
**
*/
mi_boolean
PeriodContainsNotTouches ( Period    * pPeriodOne,
			     	       Period    * pPeriodTwo,
				           MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case LT_LT_GT_GT:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodWithinNotTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first Period is completely within
**  the second.
**
**   That is, it returns MI_TRUE iff. first.start is GT second.start,
**  and first.finish is LT second.finish.
** 
*/
mi_boolean
PeriodWithinNotTouches ( Period    * pPeriodOne,
			     	     Period    * pPeriodTwo,
				         MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case GT_LT_GT_LT:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodWithinTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first Period is Within the second.
**
**  That is, it returns MI_TRUE iff. first.start is not LT second.start
**  and first.finish is not GT second.finish. This is the commutator
**  of ContainsNotTouches ( second, first ).
** 
*/
mi_boolean
PeriodWithinTouches ( Period    * pPeriodOne,
			          Period    * pPeriodTwo,
				      MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case EQ_EQ_EQ_EQ:
		case EQ_LT_EQ_LT:
		case EQ_LT_GT_EQ:
		case EQ_LT_GT_LT:
		case GT_EQ_GT_EQ:
		case GT_LT_GT_LT:
		case GT_LT_GT_EQ:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodOverlapNotTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. start of first is LT start of second, and
**   finish of first is GT finish of second.
** 
*/
mi_boolean
PeriodOverlapNotTouches ( Period    * pPeriodOne,
			         	  Period    * pPeriodTwo,
				          MI_FPARAM * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case LT_LT_GT_GT:
		case LT_LT_GT_LT:
		case GT_LT_GT_GT:
		case GT_LT_GT_LT:
			bRetVal = MI_TRUE;
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodOverlapNotTouchesWithLength
**
**   Description:  
**
**     Returns MI_TRUE iff. start of first is LT start of second, and
**   finish of first is GT finish of second. In this case, the 
**   code also computes the number of seconds in the overlap and
**   returns this as an out paramater.
**
**     Why seconds? Because there are different kinds of INTERVAL 
**   depending on the amount of time involved. 
** 
*/
mi_boolean
PeriodOverlapNotTouchesWithLength ( Period      * pPeriodOne,
			         	            Period      * pPeriodTwo,
									mi_integer  * pIntRetVal,
				                    MI_FPARAM   * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;
	Period	* pPeriodInt;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case LT_LT_GT_GT:
		case LT_LT_GT_LT:
		case GT_LT_GT_GT:
		case GT_LT_GT_LT:
		{
			/*
			**   At this point, I know that the two arguments do overlap
			**  (with touches) but I still need to compute the size of
			**  the overlap. When either the start is EPOCH, or the finish
			**  is UNBOUND of either argument, the OUT parameter will need
			**  to be a NULL value, to keep things consistent with the
			**  Length() UDF.
			*/
			bRetVal = MI_TRUE;
			pPeriodInt = new_Period();
			if ((PeriodInter (pPeriodOne,
							  pPeriodTwo,
							  pPeriodInt,
							  Gen_fparam )) == MI_OK )
			{
				(*pIntRetVal) = PeriodSizeAsDays ( pPeriodInt, Gen_fparam );
				mi_fp_setargisnull ( Gen_fparam, 2, MI_FALSE );

			} else
			{
				(*pIntRetVal) = INT_MAX;		
				mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
			}
			mi_free( pPeriodInt );
		}
		 break;
		default:
		/*
		**   Default return value is set to MI_FALSE, so there is
		**   no need to return a value here.
		**/
			mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodOverlapTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. it is not the case that [ the finish of 
**   first is LT start of second OR start of first is GT finish of 
**   second ].  This is the classic definition of Overlap, and this
**   logic is used throughout the DBMS: in the R-Tree, etc.
** 
*/
mi_boolean
PeriodOverlapTouches ( Period    * pPeriodOne,
			           Period    * pPeriodTwo,
				       MI_FPARAM * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_TRUE;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	/*  NOTE: The optimization here picks out the FALSE cases, */
	/*        which are much more common than the Overlaps.    */
	switch ( nXVal )
	{
		case  LT_LT_LT_LT:
		case  GT_GT_GT_GT:
			bRetVal = MI_FALSE;
		 break;
	}
	return bRetVal;
}
/***********************************************************************
**
**      Function:   PeriodOverlapTouchesWithLength
**
**   Description:  
**
**     Returns MI_TRUE iff. it is not the case that [ the finish of 
**   first is LT start of second OR start of first is GT finish of 
**   second ].  This is the classic definition of Overlap, and this
**   logic is used throughout the DBMS: in the R-Tree, etc. In addition,
**   this function computes the size of the overla.
** 
*/
mi_boolean
PeriodOverlapTouchesWithLength ( Period      * pPeriodOne,
			         	         Period      * pPeriodTwo,
								 mi_integer  * pIntRetVal,
				                 MI_FPARAM   * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal    = MI_FALSE;
	Period	  * pPeriodInt = 0;

	nXVal = Period_X_Period ( pPeriodOne, pPeriodTwo);
	
	switch ( nXVal )
	{
		case  LT_LT_LT_LT:
		case  GT_GT_GT_GT:
			/*
			**  Default return value is MI_FALSE, so there is no need to
			**  set it here.
			*/
			mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
		 break;
		default:
		{
			/*
			**   At this point, I know that the two arguments do overlap
			**  (with touches) but I still need to compute the size of
			**  the overlap. When either the start is EPOCH, or the finish
			**  is UNBOUND of either argument, the OUT parameter will need
			**  to be a NULL value, to keep things consistent with the
			**  Length() UDF.
			*/
			bRetVal = MI_TRUE;
			pPeriodInt = new_Period();
			if ((PeriodInter (pPeriodOne,
							  pPeriodTwo,
							  pPeriodInt,
							  Gen_fparam )) == MI_OK )
			{
				(*pIntRetVal) = PeriodSizeAsDays ( pPeriodInt, Gen_fparam );
				mi_fp_setargisnull ( Gen_fparam, 2, MI_FALSE );

			} else
			{
				(*pIntRetVal) = INT_MAX;		
				mi_fp_setargisnull (Gen_fparam, 2, MI_TRUE);
			}
			mi_free( pPeriodInt );
		}
		break;
	}
	return bRetVal;
}
/***********************************************************************
**
**     Function:  PeriodInter
**
**  Description:
**
**         Returns intersect of two Periods. If there is no
**      intersection. 
**
**/
mi_integer
PeriodInter  ( Period	 *  pPeriodOne,
			   Period	 *  pPeriodTwo,
			   Period	 *  pPeriodRet,
			   MI_FPARAM *  Gen_fparam  /* Standard info - DBDK docs. */
)
{
	mi_integer	S1S2, E1E2, S3E3, nRetVal;

	if ((S1S2 = PeriodDateCMP(&(pPeriodOne->start),
		                      &(pPeriodTwo->start))) == 1)
        pPeriodRet->start = pPeriodOne->start;
	else
        pPeriodRet->start = pPeriodTwo->start;

	if ((E1E2 = PeriodDateCMP(&(pPeriodOne->finish),
		                      &(pPeriodTwo->finish))) == -1 )
		pPeriodRet->finish = pPeriodOne->finish;
	else
	    pPeriodRet->finish = pPeriodTwo->finish;

	if ((S3E3 = PeriodDateCMP (&(pPeriodRet->start),
		                       &(pPeriodRet->finish))) < 1 )
        nRetVal = MI_OK;
	else
	{
		memset(pPeriodRet,(mi_char)NULL,sizeof(Period));
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
		if ( mi_fp_nargs( Gen_fparam ) > 2 ) 
			mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
		nRetVal = MI_ERROR;
	}

	return nRetVal;
}
/*************************************************************************
**
**   Function Name:
**
**           PeriodGetInter
**
**   About:
**
**           Returns the intersection of two Periods. If there is no
**          intersection, returns a NULL.
**
**/
Period *
PeriodGetInter ( Period	   * pPeriodOne,
			     Period	   * pPeriodTwo,
				 MI_FPARAM * Gen_fparam    )
{
	Period  * pDTRetVal;
	mi_integer   nInternalResult;

	pDTRetVal = new_Period();

	if (((nInternalResult = PeriodInter( pPeriodOne, 
		                                 pPeriodTwo,
										 pDTRetVal,
										 Gen_fparam))) == MI_ERROR )
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
		mi_free(pDTRetVal);
		pDTRetVal = (Period *)NULL;
	}
	
	return pDTRetVal;
}
/***************************************************************************
**
**     Function: PeriodUnion
**
**  Description:
**
**    Finds the UNION of two Period objects.
**          
*/
mi_integer
PeriodUnion  ( Period	 *    pPeriodOne,
			   Period	 *    pPeriodTwo,
			   Period	 *    pPeriodRet,
			   MI_FPARAM *    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	mi_integer	S1S2, E1E2;

	if ((S1S2 = PeriodDateCMP(&(pPeriodOne->start),&(pPeriodTwo->start))) 
		== -1)
	{
			memcpy(&(pPeriodRet->start),
				   &(pPeriodOne->start),
				   sizeof(mi_date));
	} else
	{
			memcpy(&(pPeriodRet->start),
				   &(pPeriodTwo->start),
				   sizeof(mi_date));
	}

	if ((E1E2 = PeriodDateCMP(&(pPeriodOne->finish),&(pPeriodTwo->finish))) 
        == 1 )
	{
			memcpy(&(pPeriodRet->finish),
				   &(pPeriodOne->finish),
				   sizeof(mi_date));
	} else
	{
			memcpy(&(pPeriodRet->finish),
				   &(pPeriodTwo->finish),
				   sizeof(mi_date));
	}
	return MI_OK;
}
/************************************************************************
**
**   Function Name:   PeriodGetUnion
**
**   About:
**
**     Compute the Union of two Period objects.
*/
Period *
PeriodGetUnion ( Period	    *   pPeriodOne,
	  			 Period	    *   pPeriodTwo,
				 MI_FPARAM	*   Gen_fparam    )
{
	Period  * pDTRetVal;
	mi_integer   nInternalResult;

	pDTRetVal = new_Period();

	nInternalResult = PeriodUnion(pPeriodOne, 
		                          pPeriodTwo,
								  pDTRetVal,
								  Gen_fparam );
	return pDTRetVal;
}
/*****************************************************************************
**
**     Function:
**
**                 PeriodSize
**
**     About:
**
**                 Used in the R-Tree support module.
**
**/
mi_integer
PeriodSize ( Period			     *    pDPArg,
			 mi_double_precision *	  pDBRetVal,
			 MI_FPARAM			 *    Gen_fparam  /* Standard info. */
)
{
	/*
	**  OK. If either end of the Period is open (Epoch, or Unbound) then
	**  return the largest possible double precision number. 
	*/
	if (( Period_Start_is_Epoch    ( pDPArg, Gen_fparam ) == MI_TRUE ) ||
		( Period_Finish_is_Unbound ( pDPArg, Gen_fparam ) == MI_TRUE ))
	{
		*pDBRetVal =  1.0E+64;  /* A very large number */
	} else
	{
		*pDBRetVal=(mi_double_precision)(pDPArg->finish - pDPArg->start);
	}

	return MI_OK;
}
/***************************************************************************
**
**  Function:  PeriodSizeAsDays
**
**     About:
**
**   This function returns the number of seconds in the interval defined
**  by the start and finish datetime values.
*/
mi_integer
PeriodSizeAsDays (  Period		*   pDPArg,
		            MI_FPARAM	*   Gen_fparam  /* Standard info. */
)
{
	mi_double_precision	dbLocal;
	mi_integer		    nRetVal;

	(void)PeriodSize( pDPArg, &(dbLocal),Gen_fparam );
	nRetVal = (mi_integer)dbLocal;
	return nRetVal;
}
/***********************************************************************
**
**      Function:   Period_P_Period
**
**   Description:  
**
**      This is a constructor function for the Period object. It
**  is meant to be minimalist. 
*/
Period *
Period_D_D ( mi_date     pDOne,
			 mi_date     pDTwo,
			 MI_FPARAM * Gen_fparam )
{
	Period  * pDPRetVal;

	pDPRetVal = new_Period();

	pDPRetVal->start  = pDOne;
	pDPRetVal->finish = pDTwo;

	Check_Period( pDPRetVal );

	return pDPRetVal;

}
/***********************************************************************
**
**    Function:   PeriodGetStart
**
** Description:
**
**            Get the start of the Period.
**/
mi_date
PeriodGetStart ( Period    * pDPArg,
		   		 MI_FPARAM * Gen_fparam )
{
	mi_date	DRetVal = 0;
	/*
	**  If the start is bounded, then return the start. Otherwise,
	**  return NULL.
	**/
	if ( Period_Start_is_Epoch ( pDPArg, Gen_fparam ) == MI_FALSE )
		DRetVal = pDPArg->start;
	else
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );

	return DRetVal;
}
/***********************************************************************
**
**    Function:   PeriodGetFinish
**
** Description:
**
**            Get the finish of the Period.
**
**/
mi_date
PeriodGetFinish ( Period    * pDPArg,
			      MI_FPARAM * Gen_fparam )
{
	mi_date  DRetVal = 0;

	/*
	**  If the finish is bounded, then return the finish. Otherwise,
	**  return NULL.
	**/
	if ( Period_Finish_is_Unbound ( pDPArg, Gen_fparam ) == MI_FALSE )
		DRetVal = pDPArg->finish;
	else
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );

	return DRetVal;
}
/*
************************************************************************
**
**  This section implements the Overlap() aggregate.
**
************************************************************************
*/
/***********************************************************************
**
**    Function: 
**
**           PeriodOverlapAggrInit  
**
** Description:
**
**           This is the initializer function for the aggregate. This
**          initializes a Period, and sets the ends to be epoch
**          and unbound.
**
**            The argument is a dummy arg used simply to identify the
**           UDF.
**/
Period_Overlap_Aggr_State *
PeriodOverlapAggrInit ( Period    * pDTDummyArg,
		                MI_FPARAM * Gen_fparam )
{
	Period_Overlap_Aggr_State * pPeriodInterState;

	pPeriodInterState = ( Period_Overlap_Aggr_State *) 
		                    mi_dalloc( sizeof(Period_Overlap_Aggr_State),
		                               PER_COMMAND);

	Set_Period_Start_Epoch_internal ( &(pPeriodInterState->Period_Value), 
		                                 Gen_fparam);
	Set_Period_Finish_Unbound_internal ( &(pPeriodInterState->Period_Value),
		                                    Gen_fparam);
    pPeriodInterState->nStatus = 0; /* initial */

	return pPeriodInterState;
}
/***********************************************************************
**
**    Function: 
**
**           PeriodOverlapAggrIter  
**
** Description:
**
**           This is the iterator function for the aggregate. It takes an
**          instance of a Period argument, and determines if there is
**          some Overlap between the pPeriodInterState and the pPeriodArg..
**/
Period_Overlap_Aggr_State *
PeriodOverlapAggrIter ( Period_Overlap_Aggr_State * pPeriodInterState,
						   Period                    * pPeriodArg,
			               MI_FPARAM                    * Gen_fparam )
{
	Period   * pPeriodLocalVar;

	if ( pPeriodInterState->nStatus != 2 ) /* If not null, process */
	{
		if (( pPeriodLocalVar = PeriodGetInter( &(pPeriodInterState->Period_Value),
													 pPeriodArg,
													 Gen_fparam)) == NULL)
		{
			pPeriodInterState->nStatus = 2; /* null */
		} else
		{
			memcpy( &(pPeriodInterState->Period_Value), 
					pPeriodLocalVar,
					sizeof(Period) );
			mi_free( pPeriodLocalVar );     /* In case this UDF is called */
											  /* in another memory duration */
			pPeriodInterState->nStatus = 1; /* OK.Valid. */
		}
	}
	return pPeriodInterState;
}
/***********************************************************************
**
**    Function: 
**
**           PeriodOverlapAggrComb  
**
** Description:
**
**           This is the combine function for the aggregate. It takes several
**          instances of Period_Overlap_Aggr_State data values, and
**          determines if there is some Overlap between them.
*/
Period_Overlap_Aggr_State *
PeriodOverlapAggrComb ( Period_Overlap_Aggr_State * pPeriodStateOne,
						   Period_Overlap_Aggr_State * pPeriodStateTwo,
			               MI_FPARAM                    * Gen_fparam )
{
	Period                    * pPeriodLocalVar;

	if (( pPeriodStateOne->nStatus == 2 ) ||
		( pPeriodStateTwo->nStatus == 2 ))
	{
			pPeriodStateOne->nStatus = 2;
	} else
	{
		if (( pPeriodLocalVar = PeriodGetInter( &(pPeriodStateOne->Period_Value),
													 &(pPeriodStateTwo->Period_Value),
													 Gen_fparam)) == NULL)
		{
			pPeriodStateOne->nStatus = 2; /* null */
		} else
		{
			memcpy( &(pPeriodStateOne->Period_Value), 
					pPeriodLocalVar,
					sizeof(Period ) );
			mi_free( pPeriodLocalVar );     /* In case this UDF is called */
											  /* in another memory duration */
			pPeriodStateOne->nStatus = 1;   /* OK.Valid. */
		}
	}
	return pPeriodStateOne;
}
/***********************************************************************
**
**    Function: 
**
**           PeriodOverlapAggrFinal  
**
** Description:
**
**           This is the finalize function for the aggregate. It takes 
**          the final instance of Period_Overlap_Aggr_State , and
**          returns either a NULL (if there are no Overlap) or it returns
**          a Period.
*/
Period *
PeriodOverlapAggrFinal ( Period_Overlap_Aggr_State * pPeriodState,
			                MI_FPARAM                    * Gen_fparam )
{
	Period * pPeriodLocalVar;

	if ( pPeriodState->nStatus == 2 )
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
		pPeriodLocalVar = (Period *) NULL;
	} else
	{
		pPeriodLocalVar= new_Period();
		memcpy( pPeriodLocalVar,  
			    &(pPeriodState->Period_Value),
				sizeof(Period ));
	}
	return pPeriodLocalVar;
}

/***********************************************************************
**
** Function name:
**
**	PeriodInput
**
** Description:
**
**	This function converts from the external representation of the
**	UDT type Period to its internal representation.   The external
**	representation is a blank-separated list of values and the
**	internal representation is a 'C' structure of type Period as
**	defined in the header file.
**
**
**	Data can be inserted into a table using an SQL insert statement:
**
**		insert into tablename values ( 'value-list' );
**
**	value-list is a space-delimited list of values.
**
**	Values stored in the mi_date format must be enclosed
**	in double quotes when entered.
**
** Special Comments:
**
**	Support routine for opaque type Period returns Period.
**
** Parameters:
**
**	mi_lvarchar *      Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	Period *                       The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    PeriodInput FunctionId: 0432f361-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/
UDREXPORT
Period *
PeriodInput
(
mi_lvarchar *                Gen_param1,                                                 /* Pointer to the input text.        */
MI_FPARAM *                  Gen_fparam                                                  /* Standard info - see DBDK docs.    */
)
{
	MI_CONNECTION *      Gen_Con;
	gl_mchar_t    *      Gen_InData;                                                 /* Pointer to the input data.        */
	gl_mchar_t    *      Gen_StartInData;                                            /* First value of Gen_InData.       */
	Period        *      Gen_OutData;                                                /* Pointer to the output data.       */
	mi_integer           Gen_DataLen;                                                /* Length of the data in bytes.      */
	Period        *      Gen_RetVal;                                                 /* The return value.                 */

	/* Get the Connection */
	Gen_Con = mi_open ( NULL, NULL, NULL );
	/* Point to the input data. */
	Gen_InData = (gl_mchar_t *)mi_get_vardata( (mi_lvarchar *) Gen_param1 );
	Gen_StartInData = Gen_InData;

	/* Get the length of the input string. */
	Gen_DataLen = mi_get_varlen( Gen_param1 );

	/* Allocate a new UDT for the return result. */
	Gen_RetVal = new_Period();
	/* Point to the output data. */
	Gen_OutData = (Period *)Gen_RetVal;

	/*
	** Check to see if the first string is "EPOCH". 
	*/
	if ( strncmp (Gen_InData,"\"EPOCH\"",6) == 0 )
	{
		Set_Period_Start_Epoch_internal ( Gen_OutData, Gen_fparam);
		Gen_InData+=7;
	} else
	{
		/* Get the mi_date value for start. */
		Gen_InData = Gen_sscanf( Gen_Con, "PeriodInput",
								 Gen_InData, 
								 Gen_DataLen - (Gen_InData - Gen_StartInData),
								 0, "%D %n", (mi_char *)&Gen_OutData->start );

	}
	while (((*Gen_InData == ' ')||(*Gen_InData == 't')||(*Gen_InData == 'o'))
		   &&(*Gen_InData != '\0')
		  )
	Gen_InData++;   /* Step over the ' to '  */

	if ( strncmp (Gen_InData,"\"FOREVER\"",9) == 0 )
	{
		Set_Period_Finish_Unbound_internal ( Gen_OutData, Gen_fparam);
	} else
	{

		/* Get the mi_date value for end. */
		Gen_InData = Gen_sscanf( Gen_Con, "PeriodInput", Gen_InData,
	                             Gen_DataLen - (Gen_InData - Gen_StartInData),
	                             0, "%D %n", (mi_char *)&Gen_OutData->finish );
	}

	/* Close the connection. */
	mi_close( Gen_Con );

	Check_Period ( Gen_RetVal );
	/* Return the UDT value. */
	return Gen_RetVal;
}
/*******************************************************************************
**
** Function name:
**
**	PeriodSend
**
** Description:
**
**	The binary send function is used to convert an instance of a
**	data type between its internal representation on the server and
**	on the client.
**
** Special Comments:
**
**	Support routine for opaque type Period returns mi_sendrecv.
**
** Parameters:
**
**	Period *        Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	mi_sendrecv *                     The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    PeriodSend FunctionId: 0432f351-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/
UDREXPORT
mi_sendrecv *
PeriodSend
(
Period *                  Gen_param1,      /* The UDT value                           */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	Period *          Gen_InData;      /* Pointer to the UDT value.               */
	Period *          Gen_OutData;     /* Pointer to the packet data.             */
	mi_sendrecv *        Gen_RetVal;      /* The return value.                       */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in PeriodSend."
		*/
		DBDK_TRACE_ERROR( "PeriodSend", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = Gen_param1;

	/* Allocate a new return value. */
	Gen_RetVal = (mi_sendrecv *)mi_new_var( sizeof( Period ) );
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in PeriodSend."
		*/
		DBDK_TRACE_ERROR( "PeriodSend", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (Period *)mi_get_vardata( (mi_lvarchar *)Gen_RetVal );

	/* Prepare the value for Gen_OutData->start. */
	mi_put_date( (mi_unsigned_char1 *)&Gen_OutData->start, &Gen_InData->start );
	
	/* Prepare the value for Gen_OutData->finish. */
	mi_put_date( (mi_unsigned_char1 *)&Gen_OutData->finish, &Gen_InData->finish );

	/* Close the connection. */
	mi_close( Gen_Con );

	/* Return the UDT for transmission. */
	return Gen_RetVal;
}
/* }}FUNCTION (#HEVO) */



/* {{FUNCTION(0432f352-98a8-11d4-aa5e-00a0c91e1580) (MergeSection) */

/*******************************************************************************
**
** Function name:
**
**	PeriodReceive
**
** Description:
**
**	The binary receive function is used to convert an instance of a
**	data type between its internal representation on the server and
**	on the client.
**
** Special Comments:
**
**	Support routine for opaque type Period returns Period.
**
** Parameters:
**
**	mi_lvarchar *      Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	Period *                       The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    PeriodReceive FunctionId: 0432f352-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/

UDREXPORT
Period *
PeriodReceive
(
mi_sendrecv *                Gen_param1,      /* The UDT value.                          */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	Period *          Gen_RetVal;      /* The return value.                       */
	Period *          Gen_InData;      /* Packet data.                            */
	Period *          Gen_OutData;     /* Output UDT value.                       */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in PeriodReceive."
		*/
		DBDK_TRACE_ERROR( "PeriodReceive", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = (Period *)mi_get_vardata( (mi_lvarchar *)Gen_param1 );


	/* Allocate room for the UDT. */
	Gen_RetVal = new_Period();
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in PeriodReceive."
		*/
		DBDK_TRACE_ERROR( "PeriodReceive", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (Period *)Gen_RetVal;

	/* Copy the attribute value(s) from the transmission parcel. */

	/* Prepare the value for Gen_OutData->start. */
	mi_get_datetime( (mi_unsigned_char1 *)&Gen_InData->start, &Gen_OutData->start );
	
	/* Prepare the value for Gen_OutData->finish. */
	mi_get_datetime( (mi_unsigned_char1 *)&Gen_InData->finish, &Gen_OutData->finish );

	/* Close the connection. */
	mi_close( Gen_Con );

	/* Return the transmitted UDT value. */
	return Gen_RetVal;
}
/* }}FUNCTION (#MBSI) */



/* {{FUNCTION(0432f354-98a8-11d4-aa5e-00a0c91e1580) (MergeSection) */

/*******************************************************************************
**
** Function name:
**
**	PeriodImportBinary
**
** Description:
**
**	The  importbinary  function is  used to  support  bulk copy of
**	binary data. This function is the same as the import function,
**	except that it  operates  on internal  representations of  the
**	data type.  This function is  usually only  provided for large
**	object  data types and is called by the LOAD BINARY command of
**	dbaccess.
**
** Special Comments:
**
**	Support routine for opaque type Period returns Period.
**
** Parameters:
**
**	mi_bitvarying *    Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	Period *                       The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    PeriodImportBinary FunctionId: 0432f354-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/

UDREXPORT
Period *
PeriodImportBinary
(
mi_bitvarying *              Gen_param1,      /* The input value.                        */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	Period *          Gen_RetVal;      /* The return result.                      */
	Period *          Gen_InData;      /* The UDT input value.                    */
	Period *          Gen_OutData;     /* The transfer data.                      */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in PeriodImportBinary."
		*/
		DBDK_TRACE_ERROR( "PeriodImportBinary", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = (Period *)mi_get_vardata( (mi_lvarchar *)Gen_param1 );

	/* Allocate a new UDT for the return result. */
	Gen_RetVal = new_Period();
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in PeriodImportBinary."
		*/
		DBDK_TRACE_ERROR( "PeriodImportBinary", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (Period *)Gen_RetVal;

	/* Prepare the value for Gen_OutData->start. */
	mi_get_datetime( (mi_unsigned_char1 *)&Gen_InData->start, &Gen_OutData->start );
	
	/* Prepare the value for Gen_OutData->finish. */
	mi_get_datetime( (mi_unsigned_char1 *)&Gen_InData->finish, &Gen_OutData->finish );

	/* Close the connection. */
	mi_close( Gen_Con );

	return Gen_RetVal;
}
/* }}FUNCTION (#9SLT) */



/* {{FUNCTION(0432f353-98a8-11d4-aa5e-00a0c91e1580) (MergeSection) */

/*******************************************************************************
**
** Function name:
**
**	PeriodExportBinary
**
** Description:
**
**	The  exportbinary  function  is  used  to  support  bulk  copy  of
**	binary  data.  This function is the same as  the export  function,
**	except that it  operates on  internal  representations of the data
**	type.  This  function is usually only  provided  for large  object
**	data types and is called by the UNLOAD BINARY command of dbaccess.
**
** Special Comments:
**
**	Support routine for opaque type Period returns mi_bitvarying.
**
** Parameters:
**
**	Period *        Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	mi_bitvarying *                   The exported UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    PeriodExportBinary FunctionId: 0432f353-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/
UDREXPORT
mi_bitvarying *
PeriodExportBinary
(
Period    *                  Gen_param1,      /* The UDT value.                          */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	mi_bitvarying *      Gen_RetVal;      /* The return value.                       */
	Period *             Gen_InData;      /* The transfer data.                      */
	Period *             Gen_OutData;     /* The output data.                        */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in PeriodExportBinary."
		*/
		DBDK_TRACE_ERROR( "PeriodExportBinary", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = Gen_param1;

	/* Allocate the output parcel. */
	Gen_RetVal = (mi_bitvarying *)mi_new_var( sizeof( Period ) );
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in PeriodExportBinary."
		*/
		DBDK_TRACE_ERROR( "PeriodExportBinary", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (Period *)mi_get_vardata( (mi_lvarchar *)Gen_RetVal );

	/* Prepare the value for Gen_OutData->start. */
	mi_put_date( (mi_unsigned_char1 *)&Gen_OutData->start, &Gen_InData->start );
	
	/* Prepare the value for Gen_OutData->finish. */
	mi_put_date( (mi_unsigned_char1 *)&Gen_OutData->finish, &Gen_InData->finish );

	/* Close the connection. */
	mi_close( Gen_Con );

	/* Return the UDT value. */
	return Gen_RetVal;
}
/* }}FUNCTION (#PIAH) */

/* {{FUNCTION(0432f357-98a8-11d4-aa5e-00a0c91e1580) (MergeSection) */

/*******************************************************************************
**
** Function name:
**
**	PeriodOutput
**
** Description:
**
**	This function converts from the internal representation of the
**	UDT type Period to its external representation.   The external
**	representation is a blank-separated list of values and the
**	internal representation is a 'C' structure of type Period as
**	defined in the header file.
**
**	Data can be retrieved from a table using an SQL select statement:
**
**		select * from tablename;
**
**	Values stored in the mi_date format must be enclosed
**	in double quotes when entered.
**
** Special Comments:
**
**	Support routine for opaque type Period returns mi_lvarchar.
**
** Parameters:
**
**	Period *        Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	mi_lvarchar *                     The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    PeriodOutput FunctionId: 0432f350-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/
UDREXPORT
mi_lvarchar *
PeriodOutput
(
Period    *   Gen_param1, /* The UDT value.                    */
MI_FPARAM *   Gen_fparam  /* Standard info - see DBDK docs.    */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	mi_integer           Gen_CharLen, i;                                                /* Estimate maximum length.          */
	Period *			 Gen_InData;                                                 /* Pointer to the input data.        */
	mi_char *            Gen_OutData;
	mi_char *            pchTempData;
    mi_char              tmpBuf[12] = "";
	mi_lvarchar *        Gen_RetVal;                                                 /* The return result.                */
	mi_integer           Gen_DataLen;                                                /* The data length.                  */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "PeriodOutput", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = Gen_param1;

	/* Compute the maximum length of the text representation. */
	Gen_CharLen         =   1       /* Leave room for the NULL terminator. */
	                     + 51       /* Add the length for start. */
	                     + 51       /* Add the length for end. */
				         + 4        /* Add the length for the " to " */
		                    ;

	/* Allocate room for the output string. */
	Gen_RetVal = mi_new_var( Gen_CharLen );
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "PeriodOutput", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = mi_get_vardata( Gen_RetVal );
	for(i=0;i<Gen_CharLen;Gen_OutData[i]=0,i++);

	/* Format the attribute value into the output string. */

	/* Wrap the value in quotes. */
	*Gen_OutData++ = '"';
	
	/* If the openning is unbound, then print out something to
	   that effect.
	*/
	if ( Period_Start_is_Epoch ( Gen_param1, Gen_fparam ) == MI_TRUE )
	{
 		sprintf( tmpBuf, "%s","EPOCH");
		pchTempData = (mi_char *)tmpBuf;
   }
	/* Convert the mi_date value from internal format. */
	else if((pchTempData = mi_date_to_string(Gen_InData->start)) == 0)
	{
		/*
		** Data conversion has failed so issue
		** the following message and quit.
		**
		** 	"Input data format error in PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "PeriodOutput", ERRORMESG11, 10 );
	
		/* not reached */
	}
	strcat( Gen_OutData, pchTempData);
	Gen_OutData+=strlen(pchTempData);
	
	/* Wrap the value in quotes. */
	*Gen_OutData++ = '"';
	
	/* Separate from the next value. */
	*Gen_OutData++ = ' ';
	*Gen_OutData++ = 't';
	*Gen_OutData++ = 'o';
	*Gen_OutData++ = ' ';

	/* Wrap the value in quotes. */
	*Gen_OutData++ = '"';
	
	/* If the finish  is unbound, then print out something to
	   that effect.
	*/
	if ( Period_Finish_is_Unbound ( Gen_param1, Gen_fparam ) == MI_TRUE )
	{
 		sprintf( tmpBuf, "%s","FOREVER");
		pchTempData = (mi_char *)tmpBuf;
    }
	/* Convert the mi_date value from internal format. */
	else if((pchTempData = mi_date_to_string(Gen_InData->finish)) == 0)
	{
		/*
		** Data conversion has failed so issue
		** the following message and quit.
		**
		** 	"Input data format error in PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "PeriodOutput", ERRORMESG11, 10 );
	
		/* not reached */
	}
	strcat(Gen_OutData, pchTempData);
	Gen_OutData+=strlen(pchTempData);
	
	/* Wrap the value in quotes. */
	*Gen_OutData++ = '"';
	
	/* Separate from the next value. */
	*Gen_OutData++ = ' ';

	/*
	** Compute the length of the data and
	** place it in the return structure.
	*/
	Gen_DataLen = (mi_integer)(Gen_OutData - mi_get_vardata( Gen_RetVal ));
	mi_set_varlen ( Gen_RetVal, Gen_DataLen );

	/* Close the connection. */
	mi_close( Gen_Con );

	/* Return the UDT value. */
	return Gen_RetVal;
}
/* }}FUNCTION (#IHO2) */


/************************************************************************
**
**                      Statistics and Selectivity.
**
**   The series of 'C' functions following implement user-defined
**  statistics and selectivity for the Period type. S/S is used by 
**  the ORDBMS query processor, and is quite complex. 
**
**   Refer to the documentation accompanying this BladeLet for details
**  on the design of this particular S/S implementation. Refer to the
**  "Extending Informix Synamic Server.2000" manual (Chapter 13) for
**  details on how S/S works in general.
**
**   First, neet to check that the right server version is present.
** The stats feature only exists in 9.20 or later, so a good-to-go
** version must be present.
*/
#ifndef mi_stat_hdrsize
#ifdef NT_MI_SAPI
#error User defined statistics is a feature of Version 9.20 of the Informix serv
er. This server, or later releases, must be installed to properly build this Dat
aBlade.
#else
Version_9_20_of_the_Informix_server_is_required.
#endif
#endif
/*
**   In an effort to simplify things, I am maintaining a minimum space
**  for the stats: less than the 256 bytes that are stored in the
**  mi_statret.mi_stat_buf. Consequently, I do not need to sweat the 
**  multi-rep complexity.
*/
/*************************************************************************
**
**  Function:
**
**     Period_StatCollect
**
**  Params:
**
**
** Period              * Gen_pColValue,    -- The column's value.
** mi_double_precision * Gen_Resolution,   -- The resolution.
** mi_double_precision * Gen_RowCount,     -- The number of rows scanned.
** MI_FPARAM           * Gen_fparam;       -- Standard info.
**
** Return value:
**
** mi_bitvarying *     -- The gathered statistical information.
**     
*/
mi_bitvarying *
Period_StatCollect
(
Period              * Gen_pColValue,
mi_double_precision * Gen_Resolution,
mi_double_precision * Gen_RowCount,
MI_FPARAM           * Gen_fparam
)
{
	mi_bitvarying     * Gen_RetVal = NULL;      /* The return value.        */
	Period_Statistics_Internal * Gen_pStatInfo; /* The statistics data.     */
	ifx_int8_t	int8LocalVal[3];				/* Scratch space for locals */

	switch( mi_fp_request( Gen_fparam ) )
	{
		case SET_INIT:                           /* Allocate and Initialize */
		{
			Gen_pStatInfo = ( Period_Statistics_Internal *) 
				             mi_dalloc(
									  sizeof(Period_Statistics_Internal),
									  PER_COMMAND
									 );
			Gen_pStatInfo->total          = 0;
			Gen_pStatInfo->null_cnt       = 0;
			Gen_pStatInfo->start_epoch    = 0;
			Gen_pStatInfo->finish_unbound = 0;
			Gen_pStatInfo->closed_cnt     = 0;
			Gen_pStatInfo->min_start      = INT_MAX;
			Gen_pStatInfo->max_finish     = INT_MIN;
			(void)ifx_int8cvlong(0, &(Gen_pStatInfo->sum_start));
			(void)ifx_int8cvlong(0, &(Gen_pStatInfo->sum_finish));
		
			/* Save in the fparam for use later. */
			mi_fp_setfuncstate( Gen_fparam, Gen_pStatInfo );

			mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );
			Gen_RetVal = NULL;

		}
		 break;
		case SET_RETONE:		   /* Compute for one row  */
		{
			/* Retrieve the pointer to the statistics info. */
			Gen_pStatInfo = (Period_Statistics_Internal *)mi_fp_funcstate( Gen_fparam );

			/* Add a new value to running statistics. */
			++Gen_pStatInfo->total;

			if( mi_fp_argisnull( Gen_fparam, 0 ) )
			{
				/*
				** The column value is NULL, so increment NULL count.
				** Nothing else needs to change.
				*/
				++Gen_pStatInfo->null_cnt;
			}
			else
			{


				if ( Period_Start_is_Epoch ( Gen_pColValue, NULL ) == MI_TRUE )
					++Gen_pStatInfo->start_epoch;
				else if ( Period_Finish_is_Unbound ( Gen_pColValue, NULL ) == MI_TRUE )
					++Gen_pStatInfo->finish_unbound;
				else {
					/*
					**  This really should be the third, or fourth, largest
					**  and smallest.
					*/
					++Gen_pStatInfo->closed_cnt;
					if ( Gen_pStatInfo->min_start > Gen_pColValue->start )
						Gen_pStatInfo->min_start = Gen_pColValue->start;

					if ( Gen_pStatInfo->max_finish < Gen_pColValue->finish )
						Gen_pStatInfo->max_finish = Gen_pColValue->finish;
		
					(void)ifx_int8cvlong ( Gen_pColValue->start, &(int8LocalVal[0]));
					(void)ifx_int8add (&(int8LocalVal[0]),
						               &(Gen_pStatInfo->sum_start),
									   &(int8LocalVal[1]));
					memcpy(&(Gen_pStatInfo->sum_start), &(int8LocalVal[1]), sizeof(ifx_int8_t));

					(void)ifx_int8cvlong ( Gen_pColValue->finish, &(int8LocalVal[0]));
					(void)ifx_int8add (&(int8LocalVal[0]),
						               &(Gen_pStatInfo->sum_finish),
									   &(int8LocalVal[1]));
					memcpy(&(Gen_pStatInfo->sum_finish), &(int8LocalVal[1]), sizeof(ifx_int8_t));
				}
			}
			mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );
			Gen_RetVal = NULL;

		}
		 break;
		case SET_END:	       /* That's all folks! */
		{
			Period_Statistics * Gen_pStatBuf;   /* The mi_stat_buf       */
			mi_statret        * Gen_stat;       /* Actual stat memory.   */
			mi_integer			Gen_StatSize;   /* Size of the struct.   */

			/* Retrieve the pointer to the statistics info. */
			Gen_pStatInfo = (Period_Statistics_Internal *)mi_fp_funcstate( Gen_fparam );

			/*
			** mi_statret is wrapped in a mi_bitvarying value.
			** and the Period_Statistics is embedded in the mi_statret,
			** at the mi_statret.mi_stat_buf
			*/
			Gen_StatSize = mi_stat_hdrsize + sizeof( Period_Statistics );
			Gen_RetVal   = (mi_bitvarying *)mi_new_var( Gen_StatSize );
			Gen_stat     = (mi_statret *)mi_get_vardata((mi_lvarchar *)Gen_RetVal );

			/* Store data inline. Will always be < 256 bytes */
			Gen_stat->szind = MI_MULTIREP_SMALL;

			/* Fill in the statistics information. */
			Gen_pStatBuf = (Period_Statistics *)Gen_stat->mi_stat_buf;

			memcpy( Gen_pStatBuf, Gen_pStatInfo, 4 * 7);

			/* Now, calculate the averages. */
			(void)ifx_int8copy  ( &(Gen_pStatInfo->sum_start), &(int8LocalVal[0]));
			(void)ifx_int8cvlong( Gen_pStatInfo->closed_cnt,
				                  &(int8LocalVal[1]));

			(void)ifx_int8div (&(int8LocalVal[0]),
							   &(int8LocalVal[1]),
						       &(int8LocalVal[2]));

			(void)ifx_int8tolong ( &(int8LocalVal[2]), &(Gen_pStatBuf->avg_start));

			/* Now, calculate the averages. */
			(void)ifx_int8copy  ( &(Gen_pStatInfo->sum_finish), &(int8LocalVal[0]));
			(void)ifx_int8cvlong( Gen_pStatInfo->closed_cnt,
				                  &(int8LocalVal[1]));

			(void)ifx_int8div (&(int8LocalVal[0]),
							   &(int8LocalVal[1]),
						       &(int8LocalVal[2]));

			(void)ifx_int8tolong (&(int8LocalVal[2]), &(Gen_pStatBuf->avg_finish));


			/* Free the allocated data. */
			mi_free( Gen_pStatInfo );

			/* This is no longer a NULL value.                */
			mi_fp_setreturnisnull( Gen_fparam, 0, MI_FALSE );

		}
		 break;

		default:			   /* Error: Big one      */
			mi_db_error_raise( NULL, MI_ERROR, 
				"Error: Period Stat Collect got bad instruction from engine.");
			/* not reached */
		 break;
	}
	return Gen_RetVal;
}

/*****************************************************************************
**
**  Function:
**
**     Period_StatPrint()
**
**  Params:
**
**
** Period              * Gen_Dummy,      -- Dummy to identify the UDF
** mi_statret          * Gen_StatsArg,   -- The stats data.
** MI_FPARAM           * Gen_fparam;       -- Standard info.
**
** Return value:
**
** mi_lvarchar *     -- A printout of the gathered statistical information.
**     
*/
UDREXPORT
mi_lvarchar *
Period_StatPrint
(
Period		  *	  Gen_Dummy,  /* Not used. */
mi_bitvarying *   Gen_vbin    /* Pointer to the statistical info.       */
)
{
	mi_statret        * pStatRetArgStats; /* Argument stats from sys catalogs */
	Period_Statistics * pPeriodStats;     /* Period Stats from stat           */
	mi_char             chFmtBuf[256];    /* Format text goes here.           */
	mi_string		  * pstrOutputs[4];

	pStatRetArgStats = (mi_statret *)mi_get_vardata((mi_lvarchar *)Gen_vbin);
	pPeriodStats     = (Period_Statistics *)&(pStatRetArgStats->mi_stat_buf);

	pstrOutputs[0] = mi_date_to_string ( pPeriodStats->min_start );
	pstrOutputs[1] = mi_date_to_string ( pPeriodStats->max_finish );
	pstrOutputs[2] = mi_date_to_string ( pPeriodStats->avg_start );
	pstrOutputs[3] = mi_date_to_string ( pPeriodStats->avg_finish );

	sprintf( chFmtBuf, "total counted = %d, null cnt = %d, min start = %s, \
max finish = %s, start epoch cnt = %d, finish unbound cnt = %d, \
closed cnt = %d, avg start = %s, avg finish = %s, avg length = %d days\n",
			pPeriodStats->total, pPeriodStats->null_cnt, pstrOutputs[0], 
			pstrOutputs[1], pPeriodStats->start_epoch, pPeriodStats->finish_unbound,
			pPeriodStats->closed_cnt, pstrOutputs[2], pstrOutputs[3], pPeriodStats->avg_finish-pPeriodStats->avg_start);

	return mi_string_to_lvarchar( chFmtBuf );

}
/*****************************************************************************
**
**   Function:
**
**		Period_ColConsSelectivity
**
**   Params:
**
**     mi_bitvarying *   -- This is a pointer to the statistics structure
**                          stored in the system catalogs. The structure
**                          reflects the distribution of data in a particular
**                          column.
**
**     pPeriodArg *      -- This is a pointer to the constant being applied
**                          by the query to the column.
**
**     mi_string  *      -- This is a pointer to a string that contains the
**                          (lower case) name of the function being applied.
**
**   Returns:
**
**   double_precision - estimate (0.0001 -> 0.9999 ) of the selectivity
**  of the supplied predicate value, against the column whose statistics are
**  passed in as the first argument, according to the function in the third
**  string.
**
**    Period_ColConsSelectivity is called by multiple UDFs which implement
**  the individual selectivity functions. This is a bit tricky, but bear
**  with it. It hurts the brain to think about it for very long.
**
********************************************************************************
*/
mi_double_precision *
Period_ColConsSelectivity
(
	mi_bitvarying *		pBinVar,
	Period        *		pPeriodArg,
	mi_string	  *		pFuncName,
	MI_FPARAM     *     Gen_Fparam
)
{
	mi_double_precision *		pdpRetVal;
	Period_Statistics   *		pPeriodStats;
	mi_statret          *		pMsr;
	mi_boolean					bStartEpoch;
	mi_boolean					bFinishForever;
	mi_double_precision			dbStartEpoch, dbFinishForever;

	pMsr         = (mi_statret *)mi_get_vardata((mi_lvarchar *)pBinVar);
	pPeriodStats = (Period_Statistics *)&(pMsr->mi_stat_buf);
	pdpRetVal    = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
	*pdpRetVal   = 0.0;
	/*
	**   pPeriodStats contains a summary of the data in the column, and
	**  pPeriodArg is a pointer to the argument constant. For each kind of
	**  UDF (passed in as a string in the third argument), this function 
	**  computes an estimate of selectity.
	*/
	if ( pFuncName == (mi_string *)NULL )
		mi_db_error_raise( NULL, MI_EXCEPTION, "Error: Null Function Name in Selectivity" );
		/* not reached */
	
	bStartEpoch    = Period_Start_is_Epoch    ( pPeriodArg, Gen_Fparam );
    bFinishForever = Period_Finish_is_Unbound ( pPeriodArg, Gen_Fparam );

	/*
	**   General notes on the approach to selectivity, which is admitedly
	**  pretty crude. A better approach would be to create a pair of
	**  histograms for the start and finish values, with special buckets
	**  for the start_epoch and finish_unbound cases. But that's a 
	**  sophisticated piece of engineering, and I lack the time to get it
	**  out in Version 1. So I will punt with something a bit more
	**  crude, but hopefully useful enough.
	**
	**   What I have to work with is an estimate of the range of Periods,
	**  (min_start to end_max) and an estimate of their size (avg_finish
	**  less avg_start). In addition, I have information about the number
	**  of periods open at either end.
	*/ 
	/*
	** So, first task is to calculate the proportion of column values
	** that are "open" at either end.
	*/
	dbStartEpoch = (mi_double_precision)pPeriodStats->start_epoch / 
		           (mi_double_precision)pPeriodStats->total;

	dbFinishForever = (mi_double_precision)pPeriodStats->finish_unbound / 
		              (mi_double_precision)pPeriodStats->total;
	/*
	**  Now, for each UDF, calculate the selectivity according to the
	**  formulae introduced above.
	*/
	if (strcmp( pFuncName, "equal" ) == 0 )
	{
		if ( bStartEpoch == MI_TRUE )  /* If the const start is epoch, then */
		{                              /* the proportion of matches in the  */
			*pdpRetVal = dbStartEpoch; /* column equals prop of start epoch.*/
			if ( bFinishForever == MI_TRUE ) /* If the finish is open, then */
				*pdpRetVal = dbStartEpoch *  /* matches is the product of   */
				             dbFinishForever;/* the start epoch and finish  */
		} else 	if ( bFinishForever == MI_TRUE ) /* forever.                */
			*pdpRetVal = dbFinishForever;
		else 
		/*
		**  Guess; say 1%. The lines of code above should catch cases where
		**  the ends are important, in which case doing an index scan might
		**  not be the best thing.
		**/
			*pdpRetVal=0.01;

	} else if (strncmp( pFuncName, "overlap", 7 ) == 0 )
	{
		/*
		** Overlap ( A, B ) 
		**
		**   To calculate Overlap, extend the start and the finish by
		**  the average period length, and then compute the proportion
		**  of objects within that range. 
		**
		**   To this value, add the ( open * closed ).
		**
		**   Special case the start is epoch and the finish forever cases.
		*/
		if ( bStartEpoch == MI_TRUE )  
			if ( bFinishForever == MI_TRUE ) 
				*pdpRetVal = 0.999;
			else
				*pdpRetVal = ((mi_double_precision)(pPeriodArg->finish -
				                                    pPeriodStats->min_start +
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)) / (0.0000001 +
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );
		else if ( bFinishForever == MI_TRUE )
			*pdpRetVal = ((mi_double_precision)(pPeriodStats->max_finish -
				                                pPeriodArg->start +
												(pPeriodStats->avg_finish - 
												 pPeriodStats->avg_start)) / (0.00000001 +
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );

		else
			*pdpRetVal = ((mi_double_precision)(pPeriodArg->finish -
				                                pPeriodArg->start + (2.0 *
												(pPeriodStats->avg_finish - 
												 pPeriodStats->avg_start))) / (0.0000001 + 
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );

	} else if (strncmp( pFuncName, "contain", 7 ) == 0 )
	{
		/*
		**  Contain ( A, B ) 
		**
		**   Special case the start is epoch and the finish forever cases.
		*/
		if ( bStartEpoch == MI_TRUE )  
			if ( bFinishForever == MI_TRUE ) 
				*pdpRetVal = 0.0;
			else
				*pdpRetVal = ((mi_double_precision)(pPeriodArg->finish -
				                                    pPeriodStats->min_start - (0.5 *
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start))) / ( 0.0000001 +
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );
		else if ( bFinishForever == MI_TRUE )
			*pdpRetVal = ((mi_double_precision)(pPeriodStats->max_finish -
				                                pPeriodArg->start - (0.5 *
												(pPeriodStats->avg_finish - 
												 pPeriodStats->avg_start))) / ( 0.0000001 + 
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );

		else
		{
			*pdpRetVal = ((mi_double_precision)(2.0 * (pPeriodStats->avg_finish - 
												       pPeriodStats->avg_start) -
												(pPeriodArg->finish -
				                                 pPeriodArg->start)) / ( 0.0000001 + 
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );
			if ( *pdpRetVal < 0.0 )	*pdpRetVal = 0.001;
			else if ( *pdpRetVal  > 1.0 )  *pdpRetVal = 0.999;

		}
		*pdpRetVal += (((mi_double_precision)pPeriodStats->start_epoch + 
				                             pPeriodStats->finish_unbound)/
						      ((mi_double_precision)pPeriodStats->total + 1.0));


	} else if (strncmp( pFuncName, "within", 6 ) == 0 )
	{
		/*
		**   Within 
		**
		**   Special case the start is epoch and the finish forever cases.
		**   To calculate Within, extend the start and the finish by
		**  half the average period length, and then compute the proportion
		**  of objects within that range. 
		**
		*/
		if ( bStartEpoch == MI_TRUE )  
			if ( bFinishForever == MI_TRUE ) 
				*pdpRetVal = 0.999;
			else
				*pdpRetVal = ((mi_double_precision)(pPeriodArg->finish -
				                                    pPeriodStats->min_start - (0.5 *
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start))) / ( 0.0000001 + 
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );
		else if ( bFinishForever == MI_TRUE )
			*pdpRetVal = ((mi_double_precision)(pPeriodStats->max_finish -
				                                pPeriodArg->start - (0.5 *
												(pPeriodStats->avg_finish - 
												 pPeriodStats->avg_start))) / ( 0.0000001 + 
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );

		else
			*pdpRetVal = ((mi_double_precision)(pPeriodArg->finish -
				                                pPeriodArg->start - 
												(pPeriodStats->avg_finish - 
												 pPeriodStats->avg_start)) / ( 0.0000001 + 
							  (mi_double_precision)(pPeriodStats->max_finish -
							                        pPeriodStats->min_start -
													(pPeriodStats->avg_finish - 
													 pPeriodStats->avg_start)
													))
		                     );


	} else if (strncmp( pFuncName, "beforetouches", 13 ) == 0 )
	{
		/*
		**  BeforeTouches ( A, B ) 
		**
		**   A.finish = B.start
		**
		**  Guess; say 1%. Probably a lot rarer, but this will trigger
		**  an index scan.
		**/
		*pdpRetVal=0.01;

	} else if (strncmp( pFuncName, "aftertouches", 12 ) == 0 )
	{
		/*
		**  AfterTouches ( A, B ) 
		**
		**   A.start = B.finish
		**
		**  Guess; say 1%. Probably a lot rarer, but this will trigger
		**  an index scan.
		**/
		*pdpRetVal=0.01;
	} else if (strncmp( pFuncName, "before", 6 ) == 0 )
	{
		/*
		**  Before ( A, B ) 
		**
		**   A.finish < B.start
        **  
		*/
		mi_double_precision dbStartLT;

		if ( bFinishForever == MI_TRUE )/* If the constant start is epoch then */
		{                               /* the number of starts in the column  */
			dbStartLT=0.999;            /* LTEQ will be the number of start    */
		} else                          /* epoch values.                       */
		{

			if ( pPeriodArg->finish < pPeriodStats->avg_start )
				dbStartLT = 0.5 * ((mi_double_precision)(pPeriodArg->finish - pPeriodStats->min_start)/
								    (0.0000001 + (mi_double_precision)(pPeriodStats->avg_start - pPeriodStats->min_start)));
			else
				dbStartLT = 0.5 + 0.5 * ((mi_double_precision)(pPeriodArg->finish - pPeriodStats->avg_start)/
										  (0.0000001 + (mi_double_precision)(pPeriodStats->max_finish-pPeriodStats->avg_finish)));

			if ( dbStartLT > 1.0 ) dbStartLT = 0.99;
			else if ( dbStartLT < 0.0 ) dbStartLT = 0.01;
		}

		*pdpRetVal=dbStartLT;

	} else if (strncmp( pFuncName, "after", 5 ) == 0 )
	{
		/*
		**  After ( A, B ) 
		**
		**   A.start > B.finish
        **  
		*/
		mi_double_precision dbStartGT;

		if ( bStartEpoch == MI_TRUE )   /* If the constant start is epoch then */
		{                               /* the number of finishes in the column*/
			dbStartGT=0.001;            /* LT this will very small.            */
		} else
		{

			if ( pPeriodArg->start < pPeriodStats->avg_start )
				dbStartGT = 0.5 * ((mi_double_precision)(pPeriodArg->start - pPeriodStats->min_start)/
								   (0.0000001 + (mi_double_precision)(pPeriodStats->avg_start - pPeriodStats->min_start)));
			else
				dbStartGT = 0.5 + 0.5 * ((mi_double_precision)(pPeriodArg->start - pPeriodStats->avg_start)/
										 (0.0000001 + (mi_double_precision)(pPeriodStats->max_finish-pPeriodStats->avg_finish)));

			dbStartGT = 1.0 - dbStartGT;
			if ( dbStartGT > 1.0 ) dbStartGT = 0.99;
			else if ( dbStartGT < 0.0 ) dbStartGT = 0.01;
		}

		*pdpRetVal=dbStartGT;
	} else
	{
		mi_string	pchBuf[128];
		sprintf( pchBuf, "Error: %s is Invalid Function for Selectivity", pFuncName);
		mi_db_error_raise( NULL, MI_EXCEPTION, pchBuf );
		/* not reached */
	}
	if ( *pdpRetVal < 0.0 )
		*pdpRetVal = 0.001;
	else if ( *pdpRetVal > 1.0 )
		*pdpRetVal = 0.999;

	return pdpRetVal;
}
/*****************************************************************************
**
**     Function:
**
**       Period_ConsConsSelectivity
**
**     About:
**
**       This is the innermost routine calculating selectivity. 
**
*/
mi_double_precision *
Period_ConsConsSelectivity
(
	Period        *		pPeriodArg1,
	Period        *		pPeriodArg2,
	mi_string	  *		pFuncName,
	MI_FPARAM     *     Gen_Fparam
)
{
	mi_double_precision * pdpRetVal;

	pdpRetVal    = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
	*pdpRetVal   = 1.0;

    if ( pFuncName == (mi_string *)NULL )
		mi_db_error_raise( NULL, MI_EXCEPTION, "Error: Null Function Name in Selectivity" );
		/* not reached */
	
	if (strcmp( pFuncName, "equal" ) == 0 )
	{
		if ( PeriodEqual ( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "overlaptouches" ) == 0 )
	{
		if ( PeriodOverlapTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "overlapnottouches" ) == 0 )
	{
		if ( PeriodOverlapNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "containstouches" ) == 0 )
	{
		if ( PeriodContainsTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "containsnottouches" ) == 0 )
	{
		if ( PeriodContainsNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "withintouches" ) == 0 )
	{
		if ( PeriodWithinTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "withinnottouches" ) == 0 )
	{
		if ( PeriodWithinNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "beforetouches" ) == 0 )
	{
		if ( PeriodBeforeTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "beforennottouches" ) == 0 )
	{
		if ( PeriodBeforeNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "aftertouches" ) == 0 )
	{
		if ( PeriodAfterTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "afternottouches" ) == 0 )
	{
		if ( PeriodAfterNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;
	} else
	{
		mi_string	pchBuf[128];
		sprintf( pchBuf, "Error: %s is Invalid Function for Selectivity", pFuncName);
		mi_db_error_raise( NULL, MI_EXCEPTION, pchBuf );
		/* not reached */
	}

	return pdpRetVal;
}
/****************************************************************************
**
**    Function:
**
**           Period_InternalSelectivity
**   Params:
**
**       2 x FUNCARG - Structure containing description of the arguments
**                     being checked.
**
**       mi_string   - Name of the function being evaluated.
**
**       MI_FPARAM   - General structure.
*/
typedef struct _typeid
{
	int2	ti_type;       /* from sqltypes.h	*/
	int2	ti_spare;      /* spare				*/
	int4	ti_xid;        /* extened type id	*/
} typeid_t;

mi_double_precision *
Period_InternalSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	mi_string  *            pFuncName,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	mi_double_precision *             Gen_RetVal;
        /* ------ {{Your_Declarations (PreserveSection) BEGIN ------ */
	mi_integer		x, y;
	MI_TYPEID *		k;
	MI_TYPEID *		l;
	typeid_t *		m;
	typeid_t *		n;
        /* ------ }}Your_Declarations (#0000) END ------ */

	/* ------ {{Your_Code (PreserveSection) BEGIN ------ */
	/* Make sure certain sapi functions exist and do the right thing. */
	x = mi_funcarg_get_routine_id( Gen_Arg1 );
	y = mi_funcarg_get_routine_id( Gen_Arg2 );

	if( x != y )
	{
		mi_db_error_raise( NULL, MI_FATAL, "Routine id's are not the same" );
	}

	k = mi_funcarg_get_datatype( Gen_Arg1 );
	l = mi_funcarg_get_datatype( Gen_Arg2 );
	m = (typeid_t *)k;
	n = (typeid_t *)l;


	if( m->ti_type != n->ti_type )
	{
		mi_db_error_raise( NULL, MI_FATAL, "Type id's are not the same" );
	}

	if( m->ti_xid != n->ti_xid )
	{
		mi_db_error_raise( NULL, MI_FATAL, "Type xid's are not the same" );
	}

	x = mi_funcarg_get_datalen( Gen_Arg1 );

	if( mi_funcarg_get_argtype( Gen_Arg1 ) == MI_FUNCARG_CONSTANT )
	{
		if( x != 16 )
		{
			mi_db_error_raise( NULL, MI_FATAL, "Length is inconsistent" );
		}
	}
	else if( x != -1 )
	{
		mi_db_error_raise( NULL, MI_FATAL, "Length should be -1" );
	}

	if( mi_funcarg_get_argtype( Gen_Arg1 ) == MI_FUNCARG_COLUMN )
	{
		x = mi_funcarg_get_colno( Gen_Arg1 );
		y = mi_funcarg_get_tabid( Gen_Arg1 );
	}

	if( mi_funcarg_get_argtype( Gen_Arg2 ) == MI_FUNCARG_COLUMN )
	{
		x = mi_funcarg_get_colno( Gen_Arg2 );
		y = mi_funcarg_get_tabid( Gen_Arg2 );
	}

	/* If we have a constant that is NULL, return 0 as selectivity. */
	if( (mi_funcarg_get_argtype( Gen_Arg1 ) == MI_FUNCARG_CONSTANT &&
		mi_funcarg_isnull( Gen_Arg1 )) ||
		(mi_funcarg_get_argtype( Gen_Arg2 ) == MI_FUNCARG_CONSTANT &&
			mi_funcarg_isnull( Gen_Arg2 )))
	{
		Gen_RetVal = (mi_double_precision *)mi_alloc( sizeof( mi_double_precision ) );
		*Gen_RetVal = 0.0;

		return ( Gen_RetVal );
	}

	/* If either argument is a "parameter," return 0.5 as the selectivity */
	if( mi_funcarg_get_argtype( Gen_Arg1 ) == MI_FUNCARG_PARAM ||
		mi_funcarg_get_argtype( Gen_Arg2 ) == MI_FUNCARG_PARAM )
	{
		Gen_RetVal = (mi_double_precision *)mi_alloc( sizeof( mi_double_precision ) );
		*Gen_RetVal = 0.5;

		return ( Gen_RetVal );
	}
	/*
	** If both are COLUMNs, then return 0.5.
	*/
	if( mi_funcarg_get_argtype( Gen_Arg1 ) == MI_FUNCARG_COLUMN &&
		mi_funcarg_get_argtype( Gen_Arg2 ) == MI_FUNCARG_COLUMN )
	{
		Gen_RetVal = (mi_double_precision *)mi_alloc( sizeof( mi_double_precision ) );
		*Gen_RetVal = 0.5;

		return ( Gen_RetVal );
	}

	/* If the first argument is constant */
	if( mi_funcarg_get_argtype( Gen_Arg1 ) == MI_FUNCARG_CONSTANT )
	{
		/* If both arguments are constants */
		if( mi_funcarg_get_argtype( Gen_Arg2 ) == MI_FUNCARG_CONSTANT )
		{
			Gen_RetVal = Period_ConsConsSelectivity
				(
					(Period *)mi_funcarg_get_constant( Gen_Arg1 ),
					(Period *)mi_funcarg_get_constant( Gen_Arg2 ),
					pFuncName,
					Gen_fparam
				);

		}
		/* If we have constant, column */
		else
		{
			Gen_RetVal = Period_ColConsSelectivity
				(
					mi_funcarg_get_distrib( Gen_Arg2 ),
					(Period *)mi_funcarg_get_constant( Gen_Arg1 ),
					pFuncName,
					Gen_fparam
				);
		}
	}
	/* If we have column, constant */
	else
	{
		return Period_ColConsSelectivity
				(
					mi_funcarg_get_distrib( Gen_Arg1 ),
					(Period *) mi_funcarg_get_constant( Gen_Arg2 ),
					pFuncName,
					Gen_fparam
				);
	}
	/* ------ }}Your_Code (#86KD) END ------ */
	return ( Gen_RetVal );
}
/*****************************************************************************
**
**  Function:
**    
**       Period_EqualSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the Equal() UDF.
**     
*/
mi_double_precision *
Period_EqualSelectivity
(
	MI_FUNCARG   *      Gen_Arg1,
	MI_FUNCARG   *      Gen_Arg2,
	MI_FPARAM    *      Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "equal", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_NotEqualSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the NotEqual() UDF. Note that it merely calls the
**    selectivity function for Equal(), and adjusts its result.
**
*/
mi_double_precision *
Period_NotEqualSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	mi_double_precision * pdbEqSel;
	mi_double_precision * pdbNEqSel;

	pdbNEqSel    = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
	pdbEqSel = Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "equal", Gen_fparam);

	*pdbNEqSel = 1.0 - (*pdbEqSel);
	mi_free( pdbEqSel );

	return pdbNEqSel;
}
/*****************************************************************************
**
**  Function:
**    
**       Period_ContainsNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the ContainsNotTouches UDF.
**
*/
mi_double_precision *
Period_ContainsNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "containsnottouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_ContainsTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the ContainsTouches UDF.
**
*/
mi_double_precision *
Period_ContainsTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "containstouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_WithinTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the WithinTouches UDF.
**
*/
mi_double_precision *
Period_WithinTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "withintouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_WithinNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the WithinNotTouches UDF.
**
*/
mi_double_precision *
Period_WithinNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "withinnottouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_OverlapTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the Overlap UDF.
**
*/
mi_double_precision *
Period_OverlapTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "overlaptouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_OverlapNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the OverlapNotTouches UDF.
**
*/
mi_double_precision *
Period_OverlapNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "overlapnottouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_BeforeTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the BeforeTouches UDF.
**
*/
mi_double_precision *
Period_BeforeTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "beforetouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_BeforeNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the BeforeNotTouches UDF.
**
*/
mi_double_precision *
Period_BeforeNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "beforenottouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_AfterTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the AfterTouches UDF.
**
*/
mi_double_precision *
Period_AfterTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "aftertouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       Period_AfterNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the AfterNotTouches UDF.
**
*/
mi_double_precision *
Period_AfterNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return Period_InternalSelectivity( Gen_Arg1, Gen_Arg2, "afternottouches", Gen_fparam);
}


#ifdef __cplusplus

}

#endif
