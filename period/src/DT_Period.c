/*
** Title:               DT_Period.c
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

/***********************************************************************
**
**   Supppoprt Routines for the DT_Period Type.
**
*/
DT_Period * 
new_DT_Period ()
{
	DT_Period * pDTPeriod;
	MI_CONNECTION * Gen_Con;
    pDTPeriod = (DT_Period *)mi_alloc( sizeof( DT_Period ) );
    if( pDTPeriod == 0 )
    {
        /*
        ** Memory allocation has failed so issue
        ** the following message and quit.
        **
        **     "Memory allocation has failed in new_DT_Period."
        */
		Gen_Con = mi_open ( NULL, NULL, NULL );
        DBDK_TRACE_ERROR( "new_DT_Period", ERRORMESG2, 10 );
        /* not reached */
    }
    pDTPeriod->start.dt_qual = TU_DTENCODE(TU_YEAR, TU_SECOND);
	pDTPeriod->finish.dt_qual= TU_DTENCODE(TU_YEAR, TU_SECOND);

	return pDTPeriod;
}

DT_Period * 
copy_DT_Period ( DT_Period * pDTPeriod)
{
	DT_Period	*	pRetDTPeriod;

	pRetDTPeriod = new_DT_Period();
	memcpy ( pRetDTPeriod, pDTPeriod, sizeof( DT_Period ) );

	return pRetDTPeriod;
}
/*****************************************************************************
**
**     Function:    DT_dtcmp
**
**  Description:  
**
**        This version of a datetime compare function is necessary because
**    of the way I have made it possible for the DT_Period to be OPEN at
**    either end. 
*/  
mi_integer 
DT_dtcmp ( mi_datetime * pMiDtOne, mi_datetime * pMiDtTwo )
{
	mi_integer	nRetVal;

	if (pMiDtOne->dt_qual == TU_DTENCODE(TU_START_EPOCH,TU_START_EPOCH))
	{
		if (pMiDtTwo->dt_qual == TU_DTENCODE(TU_START_EPOCH,TU_START_EPOCH))
		{
			nRetVal = 0;     /* Both are open at start, therefore equal */
		} else
		{
			nRetVal = -1;    /* Only first is open, so it must be LT    */
		}
	}
	else if (pMiDtOne->dt_qual == TU_DTENCODE(TU_FINISH_UNBOUND,TU_FINISH_UNBOUND))
	{
		if (pMiDtTwo->dt_qual == TU_DTENCODE(TU_FINISH_UNBOUND,TU_FINISH_UNBOUND))
		{
			nRetVal = 0;     /* Both finish unbound, so Equal           */
		} else
		{
			nRetVal = 1;     /* First finish unbound, so GT             */
		}
	}
	else /* pMiDtOne->dt_qual says this is a real value */
	{
		if (pMiDtTwo->dt_qual == TU_DTENCODE(TU_FINISH_UNBOUND, TU_FINISH_UNBOUND))
		{
			nRetVal = -1;    /* First real, second unbound, therefore less than */
		} else if (pMiDtTwo->dt_qual == TU_DTENCODE(TU_START_EPOCH,TU_START_EPOCH))
		{
			nRetVal = 1;     /* First is real, second epoch, therefore GT    */

		} else
		{
			/* Both are real (or had better be) */
			nRetVal = deccmp(&(pMiDtOne->dt_dec),&(pMiDtTwo->dt_dec));
		}
	}
	return nRetVal;
}
/****************************************************************************
**
**     Function:
**
**        Check_DT_Period
**
**  Description:
**
**        This function checks the correctness of the DT_Period object. It
**    actually does two checks. First, it ensures that the mi_datetime->dt_qual
**    is the same for both the start and finish elements, and that it is a
**    YEAR TO SECOND. Then it checks to ensure that the DT_Period->start.dt_dec
**    is not greater than the DT_Period->finish.dt_dec. 
**
*/
void 
Check_DT_Period ( DT_Period * pDTPeriod )
{
	if ((pDTPeriod->start.dt_qual != TU_DTENCODE(TU_START_EPOCH, TU_START_EPOCH)) &&
	    (pDTPeriod->finish.dt_qual!= TU_DTENCODE(TU_FINISH_UNBOUND, TU_FINISH_UNBOUND)))
	{
		if ( deccmp(&(pDTPeriod->start.dt_dec),&(pDTPeriod->finish.dt_dec)) > 0 )
		{
			mi_db_error_raise( NULL, MI_EXCEPTION, 
							   "Check Failed: start must preceed finish");
			/* not reached */
		}
	}
}
/****************************************************************************
**
**     Function:
**
**        DT_Period_Start_is_Epoch
**
**  Description:
**
**      Boolean check to determine whether or not the start of the DT_Period
**     is open.
*/
mi_boolean 
DT_Period_Start_is_Epoch ( DT_Period * pDTPeriod,
						   MI_FPARAM * Gen_fparam )
{
	if (pDTPeriod->start.dt_qual == TU_DTENCODE(TU_START_EPOCH,TU_START_EPOCH))
		return MI_TRUE;

	return MI_FALSE;
}
/****************************************************************************
**
**     Function:
**
**        DT_Period_Set_Start_Epoch
**
**  Description:
**
**       Function that sets the start of a DT_Period to Open.
**
*/
void 
Set_DT_Period_Start_Epoch_internal ( DT_Period * pDTPeriod,
						             MI_FPARAM * Gen_fparam  )
{
	pDTPeriod->start.dt_qual = TU_DTENCODE(TU_START_EPOCH,TU_START_EPOCH);
}

DT_Period * 
DT_Period_Set_Start_Epoch ( DT_Period * pDTPeriod,
						    MI_FPARAM * Gen_fparam  )
{
	DT_Period	*	pRetDTPeriod;

	/* Remember: Do not modify Arg structure. Allocate memory for the UDT */
	/* copy data there, and modify it there.                              */
	pRetDTPeriod = copy_DT_Period( pDTPeriod );
	Set_DT_Period_Start_Epoch_internal ( pRetDTPeriod, Gen_fparam );

	Check_DT_Period( pRetDTPeriod );

	return pRetDTPeriod;
}
/****************************************************************************
**
**     Function:
**
**        DT_Period_Finish_is_Unbound
**
**  Description:
**
**      Boolean check to determine whether or not the finish of the DT_Period
**     is open.
**
*/
mi_boolean 
DT_Period_Finish_is_Unbound ( DT_Period * pDTPeriod,
						      MI_FPARAM * Gen_fparam  )
{
	if (pDTPeriod->finish.dt_qual == TU_DTENCODE(TU_FINISH_UNBOUND,TU_FINISH_UNBOUND))
		return MI_TRUE;

	return MI_FALSE;
}
/****************************************************************************
**
**     Function:
**
**        DT_Period_Set_Finish_Unbound
**
**  Description:
**
**        Function that sets the finish of a DT_Period to Open.
*/
void 
Set_DT_Period_Finish_Unbound_internal ( DT_Period * pDTPeriod,
						                MI_FPARAM * Gen_fparam  )
{
	pDTPeriod->finish.dt_qual = TU_DTENCODE(TU_FINISH_UNBOUND,TU_FINISH_UNBOUND);
}

DT_Period * 
DT_Period_Set_Finish_Unbound ( DT_Period * pDTPeriod,
						       MI_FPARAM * Gen_fparam  )
{
	DT_Period	*	pRetDTPeriod;
	/* Remember: Do not modify Arg structure. Allocate memory for the UDT */
	/* copy data there, and modify it there.                              */
	pRetDTPeriod = copy_DT_Period( pDTPeriod );
	Set_DT_Period_Finish_Unbound_internal ( pRetDTPeriod, Gen_fparam );

	Check_DT_Period( pRetDTPeriod );

	return pRetDTPeriod;
}
/*******************************************************************************
**
** Function name:
**
**	DT_Period_X_DT_Period
**
** Description:
**
**   This is the guts of the DT_Period magic. This function examines the 
**  state of the two DT_Periods and returns an mi_integer that reflects
**  the relationship between them. This is called by all of the other
**  functions to determine what the relationship between the two args
**  is.
**
*/
mi_integer
DT_Period_X_DT_Period ( DT_Period  * pDTPeriodOne,
					    DT_Period  * pDTPeriodTwo )
{
	mi_integer	nRetVal = 0;
	mi_integer	S1S2, S1E2, E1S2, E1E2;
	mi_integer  arRetVals[3][3][3][3] = RANGE_X_RET_VALS;

	S1S2 = DT_dtcmp(&(pDTPeriodOne->start), &(pDTPeriodTwo->start));
	S1E2 = DT_dtcmp(&(pDTPeriodOne->start), &(pDTPeriodTwo->finish));
	E1S2 = DT_dtcmp(&(pDTPeriodOne->finish),&(pDTPeriodTwo->start));
	E1E2 = DT_dtcmp(&(pDTPeriodOne->finish),&(pDTPeriodTwo->finish));

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
		sprintf( pchErrCond, "Compare Error: arRetVals[%d][%d][%d][%d] is zero", 
			               S1S2+1, S1E2+1, E1S2+1, E1E2+1 );
		mi_db_error_raise ( NULL, MI_EXCEPTION, pchErrCond);
	}

	return nRetVal;
}
/****************************************************************************
**
**   Function name:
** 
**             DT_Period_X_to_String
**
**   About:
**
**      This is useful for diagnostic purposes. I would recommend turning it
**   off in a production system.
**/
mi_lvarchar *
DT_Period_X_to_String ( DT_Period  * pDTPeriodOne,
					    DT_Period  * pDTPeriodTwo,
				        MI_FPARAM  * Gen_fparam )
{
	mi_char	*	pchOutput;
	mi_integer  nRetVal;

	switch ( nRetVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo ) )
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
**      Function:     DT_PeriodAfterNotTouches
**
**  Description: 
**
**     Returns MI_TRUE iff. the first is completely after the second.
**
**     Returns TRUE iff.the start of the first is greater than the
**   finish of the first.
*/
mi_boolean
DT_PeriodAfterNotTouches ( DT_Period * pDTPeriodOne,
				           DT_Period * pDTPeriodTwo,
				           MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo );
	
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
**      Function:     DT_PeriodAfterTouches
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
DT_PeriodAfterTouches ( DT_Period * pDTPeriodOne,
				        DT_Period * pDTPeriodTwo,
				        MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo );
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
**      Function:     DT_PeriodBeforeNotTouches
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
DT_PeriodBeforeNotTouches ( DT_Period * pDTPeriodOne,
				            DT_Period * pDTPeriodTwo,
				            MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo );
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
**      Function:     DT_PeriodBeforeTouches
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
DT_PeriodBeforeTouches ( DT_Period * pDTPeriodOne,
				         DT_Period * pDTPeriodTwo,
				         MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo );
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
**      Function:  DT_PeriodCompare
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
DT_PeriodCompare ( DT_Period * pDTPeriodOne,
				   DT_Period * pDTPeriodTwo,
				   MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal, nRetVal;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
**      Function:  DT_PeriodEqual
**
**   Description:
**  
**  This is conistent with the DT_PeriodCompare.
**    
*/
mi_boolean
DT_PeriodEqual ( DT_Period * pDTPeriodOne,
			  	 DT_Period * pDTPeriodTwo,
				 MI_FPARAM * Gen_fparam )
{
	mi_integer	nIntCmp;
	mi_boolean	bRetVal = MI_FALSE;

	if ((nIntCmp = DT_PeriodCompare (pDTPeriodOne, 
		                             pDTPeriodTwo, 
									 Gen_fparam)) == 0 ) 
		bRetVal = MI_TRUE;

	return bRetVal;
}
/**********************************************************************
**
**      Function:  DT_PeriodNotEqual
**
**   Description:
**  
**     This is consistent with  DT_PeriodCompare.
**    
*/
mi_boolean
DT_PeriodNotEqual ( DT_Period * pDTPeriodOne,
			     	DT_Period * pDTPeriodTwo,
				    MI_FPARAM * Gen_fparam )
{
	mi_integer	nIntCmp;
	mi_boolean	bRetVal = MI_TRUE;

	if ((nIntCmp = DT_PeriodCompare (pDTPeriodOne, 
		                             pDTPeriodTwo, 
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
**      Function:   DT_PeriodContainsTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first DT_Period Contains the second.
**
**     This implements the "classic" contains, where the start or
**  the finish of the DT_Period can "touch" the start or finish of
**  the enclosing DT_Period.
**
*/
mi_boolean
DT_PeriodContainsTouches ( DT_Period * pDTPeriodOne,
			     	       DT_Period * pDTPeriodTwo,
				           MI_FPARAM * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
**      Function:   DT_PeriodContainsNotTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first DT_Period Contains the second
**  completely. That is, the start of second is GT start of first, and
**  end of first is LT end of second.
**
*/
mi_boolean
DT_PeriodContainsNotTouches ( DT_Period * pDTPeriodOne,
			     	          DT_Period * pDTPeriodTwo,
				              MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
**      Function:   DT_PeriodWithinNotTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first DT_Period is completely within
**  the second.
**
**   That is, it returns MI_TRUE iff. first.start is GT second.start,
**  and first.finish is LT second.finish.
** 
*/
mi_boolean
DT_PeriodWithinNotTouches ( DT_Period * pDTPeriodOne,
			     	        DT_Period * pDTPeriodTwo,
				            MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
**      Function:   DT_PeriodWithinTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. the first DT_Period is Within the second.
**
**  That is, it returns MI_TRUE iff. first.start is not LT second.start
**  and first.finish is not GT second.finish. This is the commutator
**  of ContainsNotTouches ( second, first ).
** 
*/
mi_boolean
DT_PeriodWithinTouches ( DT_Period * pDTPeriodOne,
			             DT_Period * pDTPeriodTwo,
				         MI_FPARAM * Gen_fparam )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
**      Function:   DT_PeriodOverlapNotTouches
**
**   Description:  
**
**     Returns MI_TRUE iff. start of first is LT start of second, and
**   finish of first is GT finish of second.
** 
*/
mi_boolean
DT_PeriodOverlapNotTouches ( DT_Period * pDTPeriodOne,
			         	     DT_Period * pDTPeriodTwo,
				             MI_FPARAM * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_FALSE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
**      Function:   DT_PeriodOverlapNotTouchesWithLength
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
DT_PeriodOverlapNotTouchesWithLength ( DT_Period   * pDTPeriodOne,
			         	               DT_Period   * pDTPeriodTwo,
									   ifx_int8_t  * pn8RetVal,
				                       MI_FPARAM   * Gen_fparam   )
{
	mi_integer	  nXVal;
	mi_boolean    bRetVal = MI_FALSE;
	DT_Period	* pDTPeriodInt;
	ifx_int8_t  * pn8Val;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
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
			pDTPeriodInt = new_DT_Period ();
			if ((DT_PeriodInter (pDTPeriodOne,
								 pDTPeriodTwo,
								 pDTPeriodInt,
								 Gen_fparam )) == MI_OK )
			{
				pn8Val = DT_PeriodSizeAsSeconds ( pDTPeriodInt, Gen_fparam );
				memcpy( pn8RetVal, pn8Val, sizeof(ifx_int8_t));
				mi_free( pn8Val );
				mi_fp_setargisnull ( Gen_fparam, 2, MI_FALSE );

			} else
			{
				(void)ifx_int8cvint( -1, pn8RetVal );
				mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
			}
			mi_free( pDTPeriodInt );
		}
		 break;
		default:
		{
			mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
		}
		 break;
	}

	return bRetVal;
}
/***********************************************************************
**
**      Function:   DT_PeriodOverlapTouches
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
DT_PeriodOverlapTouches ( DT_Period * pDTPeriodOne,
			         	  DT_Period * pDTPeriodTwo,
				          MI_FPARAM * Gen_fparam   )
{
	mi_integer	nXVal;
	mi_boolean  bRetVal = MI_TRUE;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
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
**      Function:   DT_PeriodOverlapTouchesWithLength
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
DT_PeriodOverlapTouchesWithLength ( DT_Period   * pDTPeriodOne,
			         	            DT_Period   * pDTPeriodTwo,
									ifx_int8_t  * pn8RetVal,
				                    MI_FPARAM   * Gen_fparam   )
{
	mi_integer	  nXVal;
	mi_boolean    bRetVal = MI_FALSE;
	DT_Period	* pDTPeriodInt = 0;
	ifx_int8_t	* pn8Val;

	nXVal = DT_Period_X_DT_Period ( pDTPeriodOne, pDTPeriodTwo);
	
	switch ( nXVal )
	{
		case  LT_LT_LT_LT:
		case  GT_GT_GT_GT:
		/*
		**  The default return value is MI_FALSE, so there is no
		**  need to reset it here.
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
			pDTPeriodInt = new_DT_Period();
			if ((DT_PeriodInter (pDTPeriodOne,
								 pDTPeriodTwo,
								 pDTPeriodInt,
								 Gen_fparam )) == MI_OK )
			{
				pn8Val = DT_PeriodSizeAsSeconds ( pDTPeriodInt, Gen_fparam );
				memcpy( pn8RetVal, pn8Val, sizeof(ifx_int8_t));
				mi_free( pn8Val );
				mi_fp_setargisnull ( Gen_fparam, 2, MI_FALSE );

			} else
			{
				(void)ifx_int8cvint( -1, pn8RetVal);		
				mi_fp_setargisnull ( Gen_fparam, 2, MI_TRUE );
			}
			mi_free( pDTPeriodInt );
		}
		break;
	}
	return bRetVal;
}
/***********************************************************************
**
**     Function:  DT_PeriodInter
**
**  Description:
**
**         Returns intersect of two DT_Periods. If there is no
**      intersection. 
**
**/
mi_integer
DT_PeriodInter  (	DT_Period	*    pDTPeriodOne,
				    DT_Period	*    pDTPeriodTwo,
				    DT_Period	*    pDTPeriodRet,
				    MI_FPARAM	*    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	mi_integer	S1S2, E1E2, S3E3, nRetVal;

	if ((S1S2 = DT_dtcmp(&(pDTPeriodOne->start),
		                 &(pDTPeriodTwo->start))) == 1)
	{
			memcpy(&(pDTPeriodRet->start),
				   &(pDTPeriodOne->start),
				   sizeof(mi_datetime));

	} else
	{
			memcpy(&(pDTPeriodRet->start),
				   &(pDTPeriodTwo->start),
				   sizeof(mi_datetime));
	}

	if ((E1E2 = DT_dtcmp(&(pDTPeriodOne->finish),
		                 &(pDTPeriodTwo->finish))) == -1 )
	{
			memcpy(&(pDTPeriodRet->finish),
				   &(pDTPeriodOne->finish),
				   sizeof(mi_datetime));

	} else
	{
			memcpy(&(pDTPeriodRet->finish),
				   &(pDTPeriodTwo->finish),
				   sizeof(mi_datetime));
	}

	if ((S3E3 = DT_dtcmp (&(pDTPeriodRet->start),
		                  &(pDTPeriodRet->finish))) < 1 )
	{
		nRetVal = MI_OK;
	} else
	{
		memset(pDTPeriodRet,(mi_char)NULL,sizeof(DT_Period));
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
**           DT_PeriodGetInter
**
**   About:
**
**           Returns the intersection of two DT_Periods. If there is no
**          intersection, returns a NULL.
**/
DT_Period *
DT_PeriodGetInter (	DT_Period	*    pDTPeriodOne,
			    	DT_Period	*    pDTPeriodTwo,
				    MI_FPARAM	*    Gen_fparam    )
{
	DT_Period  * pDTRetVal;
	mi_integer   nInternalResult;

	pDTRetVal = new_DT_Period();

	if (((nInternalResult = DT_PeriodInter(pDTPeriodOne, 
		                                   pDTPeriodTwo,
										   pDTRetVal,
										   Gen_fparam))) == MI_ERROR )
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
		mi_free(pDTRetVal);
		pDTRetVal = (DT_Period *)NULL;
	}
	
	return pDTRetVal;
}
/***************************************************************************
**
**     Function: DT_PeriodUnion
**
**  Description:
**
**    Finds the UNION of two DT_Period objects.
**          
**
*/
mi_integer
DT_PeriodUnion  (	DT_Period	*    pDTPeriodOne,
				    DT_Period	*    pDTPeriodTwo,
				    DT_Period	*    pDTPeriodRet,
				    MI_FPARAM	*    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	mi_integer	S1S2, E1E2;

	if ((S1S2 = DT_dtcmp(&(pDTPeriodOne->start),&(pDTPeriodTwo->start))) == -1)
	{
			memcpy(&(pDTPeriodRet->start),
				   &(pDTPeriodOne->start),
				   sizeof(mi_datetime));
	} else
	{
			memcpy(&(pDTPeriodRet->start),
				   &(pDTPeriodTwo->start),
				   sizeof(mi_datetime));
	}

	if ((E1E2 = DT_dtcmp(&(pDTPeriodOne->finish),&(pDTPeriodTwo->finish))) == 1 )
	{
			memcpy(&(pDTPeriodRet->finish),
				   &(pDTPeriodOne->finish),
				   sizeof(mi_datetime));
	} else
	{
			memcpy(&(pDTPeriodRet->finish),
				   &(pDTPeriodTwo->finish),
				   sizeof(mi_datetime));
	}
	return MI_OK;
}
/************************************************************************
**
**   Function Name:   DT_PeriodGetUnion
**
**   About:
**
**     Compute the Union of two DT_Period objects.
*/
DT_Period *
DT_PeriodGetUnion (	DT_Period	*    pDTPeriodOne,
	  			    DT_Period	*    pDTPeriodTwo,
				    MI_FPARAM	*    Gen_fparam    )
{
	DT_Period  * pDTRetVal;
	mi_integer   nInternalResult;

	pDTRetVal = new_DT_Period();

	nInternalResult = DT_PeriodUnion(pDTPeriodOne, 
		                             pDTPeriodTwo,
								     pDTRetVal,
								     Gen_fparam);
	return pDTRetVal;
}
/***********************************************************************
**
**     Function:   DT_PeriodSizeAsInterval
**
**  Description:
**
**        This function calculates the length of a DT_Period as an
**      mi_interval. If either end is open, then the size of the
**      INTERVAL is set to it's maximum possible value.
**/
mi_interval *
DT_PeriodSizeAsInterval
(	DT_Period			*    pDTArg,
	MI_FPARAM			*    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	mi_interval	*	pIntSize;
	mi_integer		nDTSubCheck;

	pIntSize = (mi_interval *)mi_alloc(sizeof(mi_interval));
	/*
	**  OK. If either end of the DT_Period is open (Epoch, or Unbound) then
	**  return NULL. 
	*/
	if (( DT_Period_Start_is_Epoch ( pDTArg, Gen_fparam ) == MI_TRUE ) ||
		( DT_Period_Finish_is_Unbound ( pDTArg, Gen_fparam ) == MI_TRUE ))
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
	} else
	{
		/*
		** Need to initialize the pIntSize to "day to fraction". From the
		** docs ( 4-16 of the DataBlade API Manual ) this is how to do it.
		**/
		pIntSize->in_qual = TU_IENCODE( 8, TU_DAY, TU_SECOND );

		if (((nDTSubCheck = dtsub( &(pDTArg->finish), 
						           &(pDTArg->start),
								   pIntSize ))) < 0 )
		{
			mi_char		pMesgBuf[80];
			sprintf("Error: dtSub( %s, %s ) yielded %d", 
				    mi_datetime_to_string( &(pDTArg->finish) ),
					mi_datetime_to_string( &(pDTArg->start) ),
					pIntSize );
			mi_db_error_raise( NULL, MI_EXCEPTION, pMesgBuf );
			/* not reached */
		}
	}
	return pIntSize;
}
/*****************************************************************************
**
**     Function:
**
**          DT_PeriodSizeInternal
**
**     About:
**
**          This function computes the number of seconds in the
**          DT_Period as an ifx_int8 value. Other sub-routines
**          call this one, and convert the ifx_int8 into more
**          hospitable formats.
**/
mi_integer
DT_PeriodSizeInternal ( DT_Period	*    pDTArg,
					    ifx_int8_t	*	 pI8RetVal,
				        MI_FPARAM	*    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	mi_interval	* pIntDiff;	
	/*
	**  OK. If either end of the DT_Period is open (Epoch, or Unbound) then
	**  return the largest possible double precision number. 
	*/
	if (( DT_Period_Start_is_Epoch ( pDTArg, Gen_fparam ) == MI_TRUE ) ||
		( DT_Period_Finish_is_Unbound ( pDTArg, Gen_fparam ) == MI_TRUE ))
	{
		(void)ifx_int8cvdbl(1.0E+48, pI8RetVal); /* Very large number */
	} else
	{
		pIntDiff = DT_PeriodSizeAsInterval ( pDTArg, Gen_fparam );
		(void)ifx_invtoInt8( pIntDiff, pI8RetVal );
	}
	return MI_OK;
}
/*
**
*/
mi_integer
DT_PeriodSize ( DT_Period			*    pDTArg,
			    mi_double_precision *	 pDBRetVal,
				MI_FPARAM			*    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	ifx_int8_t	n8RetVal;

	(void)DT_PeriodSizeInternal( pDTArg, &(n8RetVal), Gen_fparam);
	(void)ifx_int8todbl( &(n8RetVal), pDBRetVal);

	return MI_OK;
}
/***************************************************************************
**
**  Function:  DT_PeriodSizeAsSeconds
**
**     About:
**
**   This function returns the number of seconds in the interval defined
**  by the start and finish datetime values.
*/
ifx_int8_t *
DT_PeriodSizeAsSeconds (  DT_Period			*    pDTArg,
			              MI_FPARAM			*    Gen_fparam  /* Standard info - see DBDK docs. */
)
{
	ifx_int8_t	*		pn8RetVal;

	pn8RetVal = (ifx_int8_t *)mi_alloc(sizeof(ifx_int8_t));
	(void) DT_PeriodSizeInternal( pDTArg, pn8RetVal ,Gen_fparam );

	return pn8RetVal;
}
/***********************************************************************
**
**      Function:   DT_Period_DT_DT
**
**   Description:  
**
**      This is a constructor function for the DT_Period object. It
**  is meant to be minimalist. 
*/
DT_Period *
DT_Period_DT_DT (  mi_datetime * pDTOne,
				   mi_datetime * pDTTwo,
				   MI_FPARAM * Gen_fparam )
{
	DT_Period  * pDTPRetVal;

	pDTPRetVal = new_DT_Period();

	memcpy( &(pDTPRetVal->start),  pDTOne, sizeof( mi_datetime ));
	memcpy( &(pDTPRetVal->finish), pDTTwo, sizeof( mi_datetime ));

	Check_DT_Period( pDTPRetVal );

	return pDTPRetVal;
}

/***********************************************************************
**
**    Function:   DT_PeriodGetStart
**
** Description:
**
**            Get the start of the DT_Period.
**/
mi_datetime *
DT_PeriodGetStart ( DT_Period * pDTArg,
		   		    MI_FPARAM * Gen_fparam )
{
	mi_datetime * pDTRetVal;

	pDTRetVal = (mi_datetime *)mi_alloc(sizeof(mi_datetime));
	
	/*
	**  If the start is bounded, then return the start. Otherwise,
	**  return NULL.
	**/
	if ( DT_Period_Start_is_Epoch ( pDTArg, Gen_fparam ) == MI_FALSE )
	{
		memcpy( pDTRetVal, &(pDTArg->start), sizeof(mi_datetime));
	} else
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
	}

	return pDTRetVal;
}
/***********************************************************************
**
**    Function:   DT_PeriodGetFinish
**
** Description:
**
**            Get the finish of the DT_Period.
**/
mi_datetime *
DT_PeriodGetFinish ( DT_Period * pDTArg,
			   MI_FPARAM * Gen_fparam )
{
	mi_datetime * pDTRetVal;

	pDTRetVal = (mi_datetime *)mi_alloc(sizeof(mi_datetime));
	/*
	**  If the finish is bounded, then return the finish. Otherwise,
	**  return NULL.
	**/
	if ( DT_Period_Finish_is_Unbound ( pDTArg, Gen_fparam ) == MI_FALSE )
	{
		memcpy( pDTRetVal, &(pDTArg->finish), sizeof(mi_datetime));
	} else
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
	}

	return pDTRetVal;

}
/***********************************************************************
**
**  This section implements the Overlap() aggregate.
**
************************************************************************
*/
/***********************************************************************
**
**    Function: 
**
**           DT_PeriodOverlapAggrInit  
**
** Description:
**
**           This is the initializer function for the aggregate. This
**          initializes a DT_Period, and sets the ends to be epoch
**          and unbound.
**
**            The argument is a dummy arg used simply to identify the
**           UDF.
**/
DT_Period_Overlap_Aggr_State *
DT_PeriodOverlapAggrInit ( DT_Period * pDTDummyArg,
			               MI_FPARAM * Gen_fparam )
{
	DT_Period_Overlap_Aggr_State * pDTPeriodInterState;

	pDTPeriodInterState = ( DT_Period_Overlap_Aggr_State *) 
		                    mi_dalloc( sizeof(DT_Period_Overlap_Aggr_State),
		                               PER_COMMAND);

	Set_DT_Period_Start_Epoch_internal ( &(pDTPeriodInterState->DT_Period_Value), 
		                                 Gen_fparam);
	Set_DT_Period_Finish_Unbound_internal ( &(pDTPeriodInterState->DT_Period_Value),
		                                    Gen_fparam);
    pDTPeriodInterState->nStatus = 0; /* initial */

	return pDTPeriodInterState;
}
/***********************************************************************
**
**    Function: 
**
**           DT_PeriodOverlapAggrIter  
**
** Description:
**
**           This is the iterator function for the aggregate. It takes an
**          instance of a DT_Period argument, and determines if there is
**          some Overlap between the pDTPeriodInterState and the pDTPeriodArg..
**/
DT_Period_Overlap_Aggr_State *
DT_PeriodOverlapAggrIter ( DT_Period_Overlap_Aggr_State * pDTPeriodInterState,
						   DT_Period                    * pDTPeriodArg,
			               MI_FPARAM                    * Gen_fparam )
{
	DT_Period   * pDTPeriodLocalVar;

	if ( pDTPeriodInterState->nStatus != 2 ) /* If not null, process */
	{
		if (( pDTPeriodLocalVar = DT_PeriodGetInter( &(pDTPeriodInterState->DT_Period_Value),
													 pDTPeriodArg,
													 Gen_fparam)) == NULL)
		{
			pDTPeriodInterState->nStatus = 2; /* null */
		} else
		{
			memcpy( &(pDTPeriodInterState->DT_Period_Value), 
					pDTPeriodLocalVar,
					sizeof(DT_Period) );
			mi_free( pDTPeriodLocalVar );     /* In case this UDF is called */
											  /* in another memory duration */
			pDTPeriodInterState->nStatus = 1; /* OK.Valid. */
		}
	}
	return pDTPeriodInterState;
}
/***********************************************************************
**
**    Function: 
**
**           DT_PeriodOverlapAggrComb  
**
** Description:
**
**           This is the combine function for the aggregate. It takes several
**          instances of DT_Period_Overlap_Aggr_State data values, and
**          determines if there is some Overlap between them.
*/
DT_Period_Overlap_Aggr_State *
DT_PeriodOverlapAggrComb ( DT_Period_Overlap_Aggr_State * pDTPeriodStateOne,
						   DT_Period_Overlap_Aggr_State * pDTPeriodStateTwo,
			               MI_FPARAM                    * Gen_fparam )
{
	DT_Period                    * pDTPeriodLocalVar;

	if (( pDTPeriodStateOne->nStatus == 2 ) ||
		( pDTPeriodStateTwo->nStatus == 2 ))
	{
			pDTPeriodStateOne->nStatus = 2;
	} else
	{
		if (( pDTPeriodLocalVar = DT_PeriodGetInter( &(pDTPeriodStateOne->DT_Period_Value),
													 &(pDTPeriodStateTwo->DT_Period_Value),
													 Gen_fparam)) == NULL)
		{
			pDTPeriodStateOne->nStatus = 2; /* null */
		} else
		{
			memcpy( &(pDTPeriodStateOne->DT_Period_Value), 
					pDTPeriodLocalVar,
					sizeof(DT_Period ) );
			mi_free( pDTPeriodLocalVar );     /* In case this UDF is called */
											  /* in another memory duration */
			pDTPeriodStateOne->nStatus = 1;   /* OK.Valid. */
		}
	}
	return pDTPeriodStateOne;
}
/***********************************************************************
**
**    Function: 
**
**           DT_PeriodOverlapAggrFinal  
**
** Description:
**
**           This is the finalize function for the aggregate. It takes 
**          the final instance of DT_Period_Overlap_Aggr_State , and
**          returns either a NULL (if there are no Overlap) or it returns
**          a DT_Period.
*/
DT_Period *
DT_PeriodOverlapAggrFinal ( DT_Period_Overlap_Aggr_State * pDTPeriodState,
			                MI_FPARAM                    * Gen_fparam )
{
	DT_Period * pDTPeriodLocalVar;

	if ( pDTPeriodState->nStatus == 2 )
	{
		mi_fp_setreturnisnull ( Gen_fparam, 0, MI_TRUE );
		pDTPeriodLocalVar = (DT_Period *) NULL;
	} else
	{
		pDTPeriodLocalVar= new_DT_Period();
		memcpy( pDTPeriodLocalVar,  
			    &(pDTPeriodState->DT_Period_Value),
				sizeof(DT_Period ));
	}
	return pDTPeriodLocalVar;
}

/***********************************************************************
**
** Function name:
**
**	DT_PeriodInput
**
** Description:
**
**	This function converts from the external representation of the
**	UDT type DT_Period to its internal representation.   The external
**	representation is a blank-separated list of values and the
**	internal representation is a 'C' structure of type DT_Period as
**	defined in the header file.
**
**
**	Data can be inserted into a table using an SQL insert statement:
**
**		insert into tablename values ( 'value-list' );
**
**	value-list is a space-delimited list of values.
**
**	Values stored in the mi_datetime format must be enclosed
**	in double quotes when entered.
**
** Special Comments:
**
**	Support routine for opaque type DT_Period returns DT_Period.
**
** Parameters:
**
**	mi_lvarchar *      Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	DT_Period *                       The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    DT_PeriodInput FunctionId: 0432f361-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/
UDREXPORT
DT_Period *
DT_PeriodInput
(
mi_lvarchar *                Gen_param1,                                                 /* Pointer to the input text.        */
MI_FPARAM *                  Gen_fparam                                                  /* Standard info - see DBDK docs.    */
)
{
	MI_CONNECTION *      Gen_Con;
	gl_mchar_t    *      Gen_InData;                                                 /* Pointer to the input data.        */
	gl_mchar_t    *      Gen_StartInData;                                            /* First value of Gen_InData.       */
	DT_Period     *      Gen_OutData;                                                /* Pointer to the output data.       */
	mi_integer           Gen_DataLen;                                                /* Length of the data in bytes.      */
	DT_Period     *      Gen_RetVal;                                                 /* The return value.                 */

	/* Get the Connection */
	Gen_Con = mi_open ( NULL, NULL, NULL );
	/* Point to the input data. */
	Gen_InData = (gl_mchar_t *)mi_get_vardata( (mi_lvarchar *) Gen_param1 );
	Gen_StartInData = Gen_InData;

	/* Get the length of the input string. */
	Gen_DataLen = mi_get_varlen( Gen_param1 );

	/* Allocate a new UDT for the return result. */
	Gen_RetVal = new_DT_Period();
	/* Point to the output data. */
	Gen_OutData = (DT_Period *)Gen_RetVal;

	/*
	** Check to see if the first string is "EPOCH". 
	*/
	if ( strncmp (Gen_InData,"\"EPOCH\"",6) == 0 )
	{
		Set_DT_Period_Start_Epoch_internal ( Gen_OutData, Gen_fparam);
		Gen_InData+=7;
	} else
	{
		/*
		** Set the  mi_datetime  qualifier.  It is
		** important that the transfer  code above
		** match the qualifier since the qualifier
		** determines the format of the data.
		*/
		Gen_OutData->start.dt_qual = TU_DTENCODE( TU_YEAR, TU_SECOND );

		/* Get the mi_datetime value for start. */
		Gen_InData = Gen_sscanf( Gen_Con, "DT_PeriodInput",
								 Gen_InData, 
								 Gen_DataLen - (Gen_InData - Gen_StartInData),
								 0, "%T %n", (char *)&Gen_OutData->start );

	}
	while (((*Gen_InData == ' ')||(*Gen_InData == 't')||(*Gen_InData == 'o'))
		   &&(*Gen_InData != '\0')
		  )
		Gen_InData++;   /* Step over the ' to '  */

	if ( strncmp (Gen_InData,"\"FOREVER\"",9) == 0 )
	{
		Set_DT_Period_Finish_Unbound_internal ( Gen_OutData, Gen_fparam);
	} else
	{

		/*
		** Set the  mi_datetime  qualifier.
		*/
		Gen_OutData->finish.dt_qual = TU_DTENCODE( TU_YEAR, TU_SECOND );


		/* Get the mi_datetime value for end. */
		Gen_InData = Gen_sscanf( Gen_Con, "DT_PeriodInput", Gen_InData,
	                         Gen_DataLen - (Gen_InData - Gen_StartInData),
	                         0, "%T %n", (char *)&Gen_OutData->finish );
	}

	/* Close the connection. */
	mi_close( Gen_Con );

	Check_DT_Period ( Gen_RetVal );
	/* Return the UDT value. */
	return Gen_RetVal;
}
/*******************************************************************************
**
** Function name:
**
**	DT_PeriodSend
**
** Description:
**
**	The binary send function is used to convert an instance of a
**	data type between its internal representation on the server and
**	on the client.
**
** Special Comments:
**
**	Support routine for opaque type DT_Period returns mi_sendrecv.
**
** Parameters:
**
**	DT_Period *        Gen_param1;    Pointer to the input text.
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
**	    DT_PeriodSend FunctionId: 0432f351-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/
UDREXPORT
mi_sendrecv *
DT_PeriodSend
(
DT_Period *                  Gen_param1,      /* The UDT value                           */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	DT_Period *          Gen_InData;      /* Pointer to the UDT value.               */
	DT_Period *          Gen_OutData;     /* Pointer to the packet data.             */
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
		** 	"Connection has failed in DT_PeriodSend."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodSend", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = Gen_param1;

	/* Allocate a new return value. */
	Gen_RetVal = (mi_sendrecv *)mi_new_var( sizeof( DT_Period ) );
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in DT_PeriodSend."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodSend", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (DT_Period *)mi_get_vardata( (mi_lvarchar *)Gen_RetVal );

	/* Prepare the value for Gen_OutData->start. */
	mi_put_datetime( (mi_unsigned_char1 *)&Gen_OutData->start, &Gen_InData->start );
	
	/* Prepare the value for Gen_OutData->finish. */
	mi_put_datetime( (mi_unsigned_char1 *)&Gen_OutData->finish, &Gen_InData->finish );

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
**	DT_PeriodReceive
**
** Description:
**
**	The binary receive function is used to convert an instance of a
**	data type between its internal representation on the server and
**	on the client.
**
** Special Comments:
**
**	Support routine for opaque type DT_Period returns DT_Period.
**
** Parameters:
**
**	mi_lvarchar *      Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	DT_Period *                       The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    DT_PeriodReceive FunctionId: 0432f352-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/

UDREXPORT
DT_Period *
DT_PeriodReceive
(
mi_sendrecv *                Gen_param1,      /* The UDT value.                          */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	DT_Period *          Gen_RetVal;      /* The return value.                       */
	DT_Period *          Gen_InData;      /* Packet data.                            */
	DT_Period *          Gen_OutData;     /* Output UDT value.                       */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in DT_PeriodReceive."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodReceive", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = (DT_Period *)mi_get_vardata( (mi_lvarchar *)Gen_param1 );


	/* Allocate room for the UDT. */
	Gen_RetVal = new_DT_Period();
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in DT_PeriodReceive."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodReceive", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (DT_Period *)Gen_RetVal;

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
**	DT_PeriodImportBinary
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
**	Support routine for opaque type DT_Period returns DT_Period.
**
** Parameters:
**
**	mi_bitvarying *    Gen_param1;    Pointer to the input text.
**	MI_FPARAM *        Gen_fparam;    Standard info - see DBDK docs.
**
** Return value:
**
**	DT_Period *                       The constructed UDT value.
**
** History:
**
**	10/02/2000 - Generated by BladeSmith Version 4.00.TC2.
**
** Identification:
**
**	Warning: Do not remove or modify this comment:
**	    DT_PeriodImportBinary FunctionId: 0432f354-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/

UDREXPORT
DT_Period *
DT_PeriodImportBinary
(
mi_bitvarying *              Gen_param1,      /* The input value.                        */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	DT_Period *          Gen_RetVal;      /* The return result.                      */
	DT_Period *          Gen_InData;      /* The UDT input value.                    */
	DT_Period *          Gen_OutData;     /* The transfer data.                      */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in DT_PeriodImportBinary."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodImportBinary", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = (DT_Period *)mi_get_vardata( (mi_lvarchar *)Gen_param1 );

	/* Allocate a new UDT for the return result. */
	Gen_RetVal = new_DT_Period();
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in DT_PeriodImportBinary."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodImportBinary", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (DT_Period *)Gen_RetVal;

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
**	DT_PeriodExportBinary
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
**	Support routine for opaque type DT_Period returns mi_bitvarying.
**
** Parameters:
**
**	DT_Period *        Gen_param1;    Pointer to the input text.
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
**	    DT_PeriodExportBinary FunctionId: 0432f353-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/

UDREXPORT
mi_bitvarying *
DT_PeriodExportBinary
(
DT_Period *                  Gen_param1,      /* The UDT value.                          */
MI_FPARAM *                  Gen_fparam       /* Standard info - see DBDK docs.          */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	mi_bitvarying *      Gen_RetVal;      /* The return value.                       */
	DT_Period *          Gen_InData;      /* The transfer data.                      */
	DT_Period *          Gen_OutData;     /* The output data.                        */

	/* Get the current connection handle. */
	Gen_Con = mi_open( NULL, NULL, NULL );

	/* Verify that the connection has been established. */
	if( Gen_Con == 0 )
	{
		/*
		** Opening the current connection has failed
		** so issue the following message and quit.
		**
		** 	"Connection has failed in DT_PeriodExportBinary."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodExportBinary", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = Gen_param1;

	/* Allocate the output parcel. */
	Gen_RetVal = (mi_bitvarying *)mi_new_var( sizeof( DT_Period ) );
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in DT_PeriodExportBinary."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodExportBinary", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = (DT_Period *)mi_get_vardata( (mi_lvarchar *)Gen_RetVal );

	/* Prepare the value for Gen_OutData->start. */
	mi_put_datetime( (mi_unsigned_char1 *)&Gen_OutData->start, &Gen_InData->start );
	
	/* Prepare the value for Gen_OutData->finish. */
	mi_put_datetime( (mi_unsigned_char1 *)&Gen_OutData->finish, &Gen_InData->finish );

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
**	DT_PeriodOutput
**
** Description:
**
**	This function converts from the internal representation of the
**	UDT type DT_Period to its external representation.   The external
**	representation is a blank-separated list of values and the
**	internal representation is a 'C' structure of type DT_Period as
**	defined in the header file.
**
**	Data can be retrieved from a table using an SQL select statement:
**
**		select * from tablename;
**
**	Values stored in the mi_datetime format must be enclosed
**	in double quotes when entered.
**
** Special Comments:
**
**	Support routine for opaque type DT_Period returns mi_lvarchar.
**
** Parameters:
**
**	DT_Period *        Gen_param1;    Pointer to the input text.
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
**	    DT_PeriodOutput FunctionId: 0432f350-98a8-11d4-aa5e-00a0c91e1580
**
********************************************************************************
*/

UDREXPORT
mi_lvarchar *
DT_PeriodOutput
(
DT_Period *                  Gen_param1,                                                 /* The UDT value.                    */
MI_FPARAM *                  Gen_fparam                                                  /* Standard info - see DBDK docs.    */
)
{
	MI_CONNECTION *      Gen_Con;         /* The current connection.                 */
	mi_integer           Gen_CharLen;                                                /* Estimate maximum length.          */
	DT_Period *          Gen_InData;                                                 /* Pointer to the input data.        */
	char *               Gen_OutData;                                                /* Pointer to the output data.       */
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
		** 	"Connection has failed in DT_PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodOutput", ERRORMESG1, 10 );

		/* not reached */
	}

	/* Point to the input data. */
	Gen_InData = Gen_param1;

	/* Compute the maximum length of the text representation. */
	Gen_CharLen         =   1       /* Leave room for the NULL terminator. */
	               + 51             /* Add the length for start. */
	               + 51             /* Add the length for end. */
				   + 4              /* Add the length for the " to " */
		            ;

	/* Allocate room for the output string. */
	Gen_RetVal = mi_new_var( Gen_CharLen );
	if( Gen_RetVal == 0 )
	{
		/*
		** Memory allocation has failed so issue
		** the following message and quit.
		**
		** 	"Memory allocation has failed in DT_PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodOutput", ERRORMESG2, 10 );

		/* not reached */
	}

	/* Point to the output data. */
	Gen_OutData = mi_get_vardata( Gen_RetVal );

	/* Format the attribute value into the output string. */

	/* Wrap the value in quotes. */
	*Gen_OutData++ = '"';
	
	/* If the openning is unbound, then print out something to
	   that effect.
	*/
	if ( DT_Period_Start_is_Epoch ( Gen_param1, Gen_fparam ) == MI_TRUE )
	{
		sprintf( Gen_OutData, "%s", "EPOCH");
    }
	/* Convert the mi_datetime value from internal format. */
	else if( dttofmtasc( &(Gen_InData->start), Gen_OutData, 51, NULL ) )
	{
		/*
		** Data conversion has failed so issue
		** the following message and quit.
		**
		** 	"Input data format error in DT_PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodOutput", ERRORMESG11, 10 );
	
		/* not reached */
	}
	Gen_OutData += strlen( Gen_OutData );
	
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
	if ( DT_Period_Finish_is_Unbound ( Gen_param1, Gen_fparam ) == MI_TRUE )
	{
		sprintf( Gen_OutData, "%s", "FOREVER");
    }
	/* Convert the mi_datetime value from internal format. */
	else if ( dttofmtasc( &(Gen_InData->finish), Gen_OutData, 51, NULL ) )
	{
		/*
		** Data conversion has failed so issue
		** the following message and quit.
		**
		** 	"Input data format error in DT_PeriodOutput."
		*/
		DBDK_TRACE_ERROR( "DT_PeriodOutput", ERRORMESG11, 10 );
	
		/* not reached */
	}
	Gen_OutData += strlen( Gen_OutData );
	
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

/************************************************************************************
**
**  STATISTICS AND SELECTIVITY
**
*/
/************************************************************************
**
**                      Statistics and Selectivity.
**
**   The series of 'C' functions following implement user-defined
**  statistics and selectivity for the DT_Period type. S/S is used by 
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
**     DT_PeriodStatCollect
**
**  Params:
**
**
** DT_Period           * Gen_pColValue,    -- The column's value.
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
DT_PeriodStatCollect
(
DT_Period           * Gen_pColValue,
mi_double_precision * Gen_Resolution,
mi_double_precision * Gen_RowCount,
MI_FPARAM           * Gen_fparam
)
{
	mi_bitvarying     * Gen_RetVal = NULL;      /* The return value.        */
	DT_Period_Statistics * Gen_pStatInfo; /* The statistics data.     */

	switch( mi_fp_request( Gen_fparam ) )
	{
		case SET_INIT:                           /* Allocate and Initialize */
		{
			mi_integer	nRetChk;
			Gen_pStatInfo = ( DT_Period_Statistics *) 
				             mi_dalloc(
									  sizeof(DT_Period_Statistics),
									  PER_COMMAND
									 );
			Gen_pStatInfo->total          = 0;
			Gen_pStatInfo->null_cnt       = 0;
			Gen_pStatInfo->start_epoch    = 0;
			Gen_pStatInfo->finish_unbound = 0;
			Gen_pStatInfo->closed_cnt     = 0;
			/*
			** According to the SQL Reference Manual, this is the largest legal
			** DATETIME value. The idea is that *every* date or datetime will
			** be less than this, *unless* someone decides to make the 
			** maximum legal data bigger, for some fool reason.
			**
			**  Note that there are about 473,352,335,999 seconds between 5000 BC
			** and this date. So a table with 1,000,000 rows, all of the
			** absolute maximum, would still fit into an int8.
			**/
			Gen_pStatInfo->min_start.dt_qual = TU_DTENCODE(TU_YEAR, TU_SECOND);
			if ((nRetChk = dtcvasc("9999-12-31 23:59:59",
				                    &(Gen_pStatInfo->min_start))) != 0 )
			{
				mi_char pchErrBuf[80];
				sprintf(pchErrBuf,"dtcvasc() error %d in StatCollect.", nRetChk);
				mi_db_error_raise( NULL, MI_EXCEPTION, pchErrBuf );
				/* not reached */
			}
			(void)ifx_int8cvint((mi_integer)0, &(Gen_pStatInfo->max_finish));
			(void)ifx_int8cvint((mi_integer)0, &(Gen_pStatInfo->avg_start));
			(void)ifx_int8cvint((mi_integer)0, &(Gen_pStatInfo->avg_finish));
		
			/* Save in the fparam for use later. */
			mi_fp_setfuncstate( Gen_fparam, Gen_pStatInfo );

			mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );
			Gen_RetVal = NULL;

		}
		 break;
		case SET_RETONE:		   /* Compute for one row  */
		{
			/* Retrieve the pointer to the statistics info. */
			Gen_pStatInfo = (DT_Period_Statistics *)mi_fp_funcstate( Gen_fparam );

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
				if ( DT_Period_Start_is_Epoch ( Gen_pColValue, NULL ) == MI_TRUE )
					++Gen_pStatInfo->start_epoch;
				else if ( DT_Period_Finish_is_Unbound ( Gen_pColValue, NULL ) == MI_TRUE )
					++Gen_pStatInfo->finish_unbound;
				else 
				{
					ifx_int8_t *	pn8Diff;
					ifx_int8_t		pn8Total, pn8Change;

					++Gen_pStatInfo->closed_cnt;

					if ( DT_dtcmp ( &(Gen_pColValue->start), &(Gen_pStatInfo->min_start) ) < 0 )
					{
						/*
						** The new Period minimum is less than the current minimum. So
						** I need to adjust the minimum, and then re-callibrate all
						** of the other values by Diff * Count.
						*/
						pn8Diff = DT_PeriodSizeAsSeconds ( DT_Period_DT_DT ( &(Gen_pColValue->start),
																			 &(Gen_pStatInfo->min_start),
																			 Gen_fparam ),
							                               Gen_fparam );
						(void)ifx_int8cvint( Gen_pStatInfo->closed_cnt, &(pn8Total));
						(void)ifx_int8mul( &(pn8Total), pn8Diff, &(pn8Change));
						(void)ifx_int8add( &(Gen_pStatInfo->avg_start), &(pn8Change), &(Gen_pStatInfo->avg_start));
						(void)ifx_int8add( &(Gen_pStatInfo->avg_finish), &(pn8Change), &(Gen_pStatInfo->avg_finish));
						memcpy(&(Gen_pStatInfo->min_start), &(Gen_pColValue->start), sizeof(mi_datetime));
						mi_free( pn8Diff );
					} 
					pn8Diff = DT_PeriodSizeAsSeconds ( DT_Period_DT_DT ( &(Gen_pStatInfo->min_start),
							                                             &(Gen_pColValue->start),
																		 Gen_fparam ),
							                               Gen_fparam );
					(void)ifx_int8add( &(Gen_pStatInfo->avg_start), pn8Diff, &(Gen_pStatInfo->avg_start));
					mi_free( pn8Diff );

					pn8Diff = DT_PeriodSizeAsSeconds ( DT_Period_DT_DT ( &(Gen_pStatInfo->min_start),
							                                             &(Gen_pColValue->finish),
																		 Gen_fparam ),
							                               Gen_fparam );

					(void)ifx_int8add( &(Gen_pStatInfo->avg_finish), pn8Diff, &(Gen_pStatInfo->avg_finish));
					if ( ifx_int8cmp(&(Gen_pStatInfo->max_finish), pn8Diff ) < 0 )
						memcpy( &(Gen_pStatInfo->max_finish), pn8Diff, sizeof(ifx_int8_t));

					mi_free( pn8Diff );
				}
			}

			mi_fp_setreturnisnull( Gen_fparam, 0, MI_TRUE );
			Gen_RetVal = NULL;
		}
		 break;
		case SET_END:	       /* That's all folks! */
		{
			DT_Period_Statistics * Gen_pStatBuf;   /* The mi_stat_buf       */
			mi_statret           * Gen_stat;       /* Actual stat memory.   */
			mi_integer		   	   Gen_StatSize;   /* Size of the struct.   */
			ifx_int8_t			   n8TotDivisor;

			/* Retrieve the pointer to the statistics info. */
			Gen_pStatInfo = (DT_Period_Statistics *)mi_fp_funcstate( Gen_fparam );

			/*
			** mi_statret is wrapped in a mi_bitvarying value.
			** and the DT_Period_Statistics is embedded in the mi_statret,
			** at the mi_statret.mi_stat_buf
			*/
			Gen_StatSize = mi_stat_hdrsize + sizeof( DT_Period_Statistics );
			Gen_RetVal   = (mi_bitvarying *)mi_new_var( Gen_StatSize );
			Gen_stat     = (mi_statret *)mi_get_vardata((mi_lvarchar *)Gen_RetVal );

			/* Store data inline. Will always be < 256 bytes */
			Gen_stat->szind = MI_MULTIREP_SMALL;

			/* Fill in the statistics information. */
			Gen_pStatBuf = (DT_Period_Statistics *)Gen_stat->mi_stat_buf;

			(void)ifx_int8cvint( Gen_pStatInfo->closed_cnt, &(n8TotDivisor));
			(void)ifx_int8div( &(Gen_pStatInfo->avg_start), &(n8TotDivisor), &(Gen_pStatInfo->avg_start));
			(void)ifx_int8div( &(Gen_pStatInfo->avg_finish), &(n8TotDivisor), &(Gen_pStatInfo->avg_finish));

			memcpy( Gen_pStatBuf, Gen_pStatInfo, sizeof(DT_Period_Statistics));

			/* This is no longer a NULL value.                */
			mi_fp_setreturnisnull( Gen_fparam, 0, MI_FALSE );

		}
		 break;

		default:			   /* Error: Big one      */
			mi_db_error_raise( NULL, MI_ERROR, 
				"Error: DT_Period Stat Collect got bad instruction from engine.");
			/* not reached */
		 break;
	}
	return Gen_RetVal;
}
/*****************************************************************************
**
**  Function:
**
**     DT_PeriodStatPrint
**
**  Params:
**
**
** DT_Period           * Gen_Dummy,      -- Dummy to identify the UDF
** mi_statret          * Gen_StatsArg,   -- The stats data.
** MI_FPARAM           * Gen_fparam;     -- Standard info.
**
** Return value:
**
** mi_lvarchar *     -- A printout of the gathered statistical information.
**     
*/
mi_lvarchar *
DT_PeriodStatPrint
(
	DT_Period		  *	  Gen_Dummy,  /* Not used. */
	mi_bitvarying     *   Gen_vbin    /* Pointer to the statistical info.       */
)
{
	mi_statret           * pStatRetArgStats; /* Argument stats from sys catalogs */
	DT_Period_Statistics * pPeriodStats;     /* DT_Period Stats from stat        */
	mi_char                chFmtBuf[256];    /* Format text goes here.           */
	mi_datetime			   dtTemp;
	mi_interval			   invTemp;
	mi_string		     * pstrOutputs[4];

	pStatRetArgStats = (mi_statret *)mi_get_vardata((mi_lvarchar *)Gen_vbin);
	pPeriodStats     = (DT_Period_Statistics *)&(pStatRetArgStats->mi_stat_buf);
	invTemp.in_qual  = TU_IENCODE(8, TU_DAY, TU_SECOND );
	dtTemp.dt_qual   = TU_DTENCODE( TU_YEAR, TU_SECOND);

	pstrOutputs[0] = mi_datetime_to_string ( &(pPeriodStats->min_start) );

	(void)ifx_int8toInterval(&(pPeriodStats->max_finish),&(invTemp));
	(void)dtaddinv( &(pPeriodStats->min_start), &(invTemp), &(dtTemp));
	pstrOutputs[1] = mi_datetime_to_string ( &(dtTemp) );

	(void)ifx_int8toInterval(&(pPeriodStats->avg_start),&(invTemp));
	(void)dtaddinv( &(pPeriodStats->min_start), &(invTemp), &(dtTemp));
	pstrOutputs[2] = mi_datetime_to_string ( &(dtTemp) );

	(void)ifx_int8toInterval(&(pPeriodStats->avg_finish),&(invTemp));
	(void)dtaddinv( &(pPeriodStats->min_start), &(invTemp), &(dtTemp));
	pstrOutputs[3] = mi_datetime_to_string ( &(dtTemp) );

	sprintf( chFmtBuf, "total counted = %d, null cnt = %d, min start = %s, \
max finish = %s, start epoch cnt = %d, finish unbound cnt = %d, \
closed cnt = %d, avg start = %s, avg finish = %s\n",
			pPeriodStats->total, pPeriodStats->null_cnt, pstrOutputs[0], 
			pstrOutputs[1], pPeriodStats->start_epoch, pPeriodStats->finish_unbound,
			pPeriodStats->closed_cnt, pstrOutputs[2], pstrOutputs[3]);

	return mi_string_to_lvarchar( chFmtBuf );

}
/*****************************************************************************
**
**   Function:
**
**		DT_Period_ColConsSelectivity
**
**   Params:
**
**     mi_bitvarying *   -- This is a pointer to the statistics structure
**                          stored in the system catalogs. The structure
**                          reflects the distribution of data in a particular
**                          column.
**
**     DT_Period    *    -- This is a pointer to the constant being applied
**                          by the query to the column.
**
**     mi_string    *    -- This is a pointer to a string that contains the
**                          (lower case) name of the function being applied.
**
**   Returns:
**
**   double_precision - estimate (0.0001 -> 0.9999 ) of the selectivity
**  of the supplied predicate value, against the column whose statistics are
**  passed in as the first argument, according to the function in the third
**  string.
**
**    DT_Period_ColConsSelectivity is called by multiple UDFs which implement
**  the individual selectivity functions. This is a bit tricky, but bear
**  with it. It hurts the brain to think about it for very long.
**
********************************************************************************
*/
mi_double_precision *
DT_Period_ColConsSelectivity
(
	mi_bitvarying  *	pBinVar,
	DT_Period      *	pPeriodArg,
	mi_string	   *	pFuncName,
	MI_FPARAM      *    Gen_Fparam
)
{
	mi_double_precision    *	pdpRetVal;
	DT_Period_Statistics   *	pPeriodStats;
	mi_statret             *	pMsr;
	mi_boolean					bStartEpoch;
	mi_boolean					bFinishForever;
	mi_double_precision			dbStartEpoch, dbFinishForever, dbStart, dbFinish;
	mi_double_precision			dbAvgStart, dvAvgFinish, dbMaxFinish, dbAvgLen, dbMaxLen;

	pMsr         = (mi_statret *)mi_get_vardata((mi_lvarchar *)pBinVar);
	pPeriodStats = (DT_Period_Statistics *)&(pMsr->mi_stat_buf);
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
	
	if ((bStartEpoch = DT_Period_Start_is_Epoch  ( pPeriodArg, Gen_Fparam )) == MI_FALSE)
	{
		(void)ifx_int8todbl( DT_PeriodSizeAsSeconds( 
		                     ( DT_Period_DT_DT ( &(pPeriodStats->min_start),
						                         &(pPeriodArg->start),
							  	   			     Gen_Fparam)),
							   Gen_Fparam
						     ),
		                     &(dbStart)
						   );

	} else
	{
		dbStart = 0.0;  /* Code always checks for bStartEpoch, so this value is unused.*/
	}

    if (( bFinishForever = DT_Period_Finish_is_Unbound ( pPeriodArg, Gen_Fparam )) == MI_FALSE )
	{
		(void)ifx_int8todbl( DT_PeriodSizeAsSeconds( 
		                        ( DT_Period_DT_DT ( &(pPeriodStats->min_start),
						                            &(pPeriodArg->finish),
							  	 			        Gen_Fparam)),
							      Gen_Fparam
						        ),
		                        &(dbFinish)
						   );
	} else
	{
		dbFinish = 0.0;  /* Code always checks for bFinishForever, so this value is unused.*/
	}

	(void)ifx_int8todbl( &(pPeriodStats->avg_start),  &(dbAvgStart));
	(void)ifx_int8todbl( &(pPeriodStats->avg_finish), &(dvAvgFinish));
	(void)ifx_int8todbl( &(pPeriodStats->max_finish), &(dbMaxFinish));
	dbAvgLen = dvAvgFinish - dbAvgStart;
	dbMaxLen = dbMaxFinish;
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
    **
    **   So, first task is to calculate the proportion of column values
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
				*pdpRetVal = ((dbFinish + dbAvgLen ) / (dbMaxLen - dbAvgLen));
		else if ( bFinishForever == MI_TRUE )
				*pdpRetVal = ((dbMaxLen - dbStart - dbAvgLen ) / (dbMaxLen - dbAvgLen));
		else
				*pdpRetVal = ((dbFinish - dbStart + 2.0 * dbAvgLen ) /(dbMaxLen - dbAvgLen));

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
				*pdpRetVal = (dbFinish - (0.5 * dbAvgLen ) ) / (dbMaxLen - dbAvgLen);
				
		else if ( bFinishForever == MI_TRUE )
			*pdpRetVal = (dbMaxLen - dbStart - (0.5 * dbAvgLen )) / (dbMaxLen - dbAvgLen);
		else
		{
			*pdpRetVal = ((2.0 * dbAvgLen ) - ( dbFinish - dbStart ) ) / (dbMaxLen - dbAvgLen);
				
			if ( *pdpRetVal < 0.0 )	*pdpRetVal = 0.001;
			else if ( *pdpRetVal  > 1.0 )  *pdpRetVal = 0.999;

		}
		*pdpRetVal += (((mi_double_precision)pPeriodStats->start_epoch + 
				                             pPeriodStats->finish_unbound)/
						      ((mi_double_precision)pPeriodStats->total));

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
		{
			if ( bFinishForever == MI_TRUE ) 
				*pdpRetVal = 0.999;
			else
				*pdpRetVal = ((dbFinish - ( 0.5 * dbAvgLen ))/(dbMaxLen - dbAvgLen));
		}		
		else if ( bFinishForever == MI_TRUE )
			*pdpRetVal = ((dbFinish - dbStart - ( 0.5 * dbAvgLen ))/(dbMaxLen - dbAvgLen));
			
		else
			*pdpRetVal = ((dbFinish - dbStart - dbAvgLen) / (dbMaxLen - dbAvgLen));

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

		if ( bStartEpoch == MI_TRUE )   /* If the constant start is epoch then */
		{                               /* the number of starts in the column  */
			*pdpRetVal= 0.0;            /* LTEQ will be the number of start    */
		} else                          /* epoch values.                       */
		{
			*pdpRetVal = (dbStart) / (dbMaxLen - dbAvgLen);
		}
	} else if (strncmp( pFuncName, "after", 5 ) == 0 )
	{
		/*
		**  After ( A, B ) 
		**
		**   A.start > B.finish
        **  
		*/
		if ( bFinishForever == MI_TRUE )/* If the constant finish is forever then */
		{                               /* the number of finishes in the column   */
			*pdpRetVal=0.001;           /* LT this will very small.               */
		} else
		{
			*pdpRetVal = (dbStart) / (dbMaxLen - dbAvgLen);
		}
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
/****************************************************************************
**
**     Function:
**
**       DT_Period_ConsConsSelectivity
**
**     About:
**
**       For cases where both arguments to the selectivity function are 
**    constants. Can compute this one directly. 
**
*/
mi_double_precision *
DT_Period_ConsConsSelectivity
(
	DT_Period     *		pPeriodArg1,
	DT_Period     *		pPeriodArg2,
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
		if ( DT_PeriodEqual ( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "overlaptouches" ) == 0 )
	{
		if ( DT_PeriodOverlapTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "overlapnottouches" ) == 0 )
	{
		if ( DT_PeriodOverlapNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "containstouches" ) == 0 )
	{
		if ( DT_PeriodContainsTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "containsnottouches" ) == 0 )
	{
		if ( DT_PeriodContainsNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "withintouches" ) == 0 )
	{
		if ( DT_PeriodWithinTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "withinnottouches" ) == 0 )
	{
		if ( DT_PeriodWithinNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "beforetouches" ) == 0 )
	{
		if ( DT_PeriodBeforeTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "beforennottouches" ) == 0 )
	{
		if ( DT_PeriodBeforeNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "aftertouches" ) == 0 )
	{
		if ( DT_PeriodAfterTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
			*pdpRetVal = 0.0;

	} else if ( strcmp( pFuncName, "afternottouches" ) == 0 )
	{
		if ( DT_PeriodAfterNotTouches( pPeriodArg1, pPeriodArg2, Gen_Fparam) == MI_FALSE )
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
**           DT_PeriodInternalSelectivity
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
DT_PeriodInternalSelectivity
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
			Gen_RetVal = DT_Period_ConsConsSelectivity
				(
					(DT_Period *)mi_funcarg_get_constant( Gen_Arg1 ),
					(DT_Period *)mi_funcarg_get_constant( Gen_Arg2 ),
					pFuncName,
					Gen_fparam
				);

		}
		/* If we have constant, column */
		else
		{
			Gen_RetVal = DT_Period_ColConsSelectivity
				(
					mi_funcarg_get_distrib( Gen_Arg2 ),
					(DT_Period *)mi_funcarg_get_constant( Gen_Arg1 ),
					pFuncName,
					Gen_fparam
				);
		}
	}
	/* If we have column, constant */
	else
	{
		return DT_Period_ColConsSelectivity
				(
					mi_funcarg_get_distrib( Gen_Arg1 ),
					(DT_Period *) mi_funcarg_get_constant( Gen_Arg2 ),
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
**       DT_PeriodEqualSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the Equal() UDF.
**     
*/
mi_double_precision *
DT_PeriodEqualSelectivity
(
	MI_FUNCARG   *      Gen_Arg1,
	MI_FUNCARG   *      Gen_Arg2,
	MI_FPARAM    *      Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, "equal", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       DT_PeriodNotEqualSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the NotEqual() UDF. Note that it merely calls the
**    selectivity function for Equal(), and adjusts its result.
**
*/
mi_double_precision *
DT_PeriodNotEqualSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	mi_double_precision * pdbEqSel;
	mi_double_precision * pdbNEqSel;

	pdbNEqSel = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
	pdbEqSel  = DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, "equal", Gen_fparam);

	*pdbNEqSel = 1.0 - (*pdbEqSel);
	mi_free( pdbEqSel );

	return pdbNEqSel;
}
/****************************************************************************
**
**  Function:
**    
**       DT_PeriodContainsNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the ContainsNotTouches UDF.
**
*/
mi_double_precision *
DT_PeriodContainsNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                 "containsnottouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       DT_PeriodContainsTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the ContainsTouches UDF.
**
*/
mi_double_precision *
DT_PeriodContainsTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2,
		                                 "containstouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       DT_PeriodWithinTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the WithinTouches UDF.
**
*/
mi_double_precision *
DT_PeriodWithinTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                 "withintouches", Gen_fparam);
}
/*****************************************************************************
**
**  Function:
**    
**       DT_PeriodWithinNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the WithinNotTouches UDF.
**
*/
mi_double_precision *
DT_PeriodWithinNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM *             Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                 "withinnottouches", Gen_fparam);
}
/****************************************************************************
**
**  Function:
**    
**       DT_PeriodOverlapTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the Overlap UDF.
**
*/
mi_double_precision *
DT_PeriodOverlapTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                  "overlaptouches", Gen_fparam);
}
/****************************************************************************
**
**  Function:
**    
**       DT_PeriodOverlapNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the OverlapNotTouches UDF.
**
*/
mi_double_precision *
DT_PeriodOverlapNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                  "overlapnottouches", Gen_fparam);
}
/****************************************************************************
**
**  Function:
**    
**       DT_PeriodBeforeTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the BeforeTouches UDF.
**
*/
mi_double_precision *
DT_PeriodBeforeTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                  "beforetouches", Gen_fparam);
}
/****************************************************************************
**
**  Function:
**    
**       DT_PeriodBeforeNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the BeforeNotTouches UDF.
**
*/
mi_double_precision *
DT_PeriodBeforeNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2,
		                                  "beforenottouches", Gen_fparam);
}
/****************************************************************************
**
**  Function:
**    
**       DT_PeriodAfterTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the AfterTouches UDF.
**
*/
mi_double_precision *
DT_PeriodAfterTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                  "aftertouches", Gen_fparam);
}
/***************************************************************************
**
**  Function:
**    
**       DT_PeriodAfterNotTouchesSelectivity
**
**  About:
**
**       This is the public function that is called to determine the
**    selectivity of the AfterNotTouches UDF.
**
*/
mi_double_precision *
DT_PeriodAfterNotTouchesSelectivity
(
	MI_FUNCARG *            Gen_Arg1,
	MI_FUNCARG *            Gen_Arg2,
	MI_FPARAM  *            Gen_fparam        /* Standard info - see DBDK docs. */
)
{
	return DT_PeriodInternalSelectivity( Gen_Arg1, Gen_Arg2, 
		                                 "afternottouches", Gen_fparam);
}


#ifdef __cplusplus

}

#endif
