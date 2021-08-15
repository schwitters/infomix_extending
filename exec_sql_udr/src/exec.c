/******************************************************************************
**
**   File:   exec.c
**
**   About:
**
**   This is the principle source file for the "exec" BladeLet. This BladeLet
**   has two principle UDRs:
**
**   Exec (  lvarchar ) -> lvarchar
**
**   Exec_for_Rows ( lvarchar ) -> lvarchar AS ITERATOR
**
**   These two functions expect that their arguments are SQL queries, which 
**   they run against the current schema. Depending on the kind of query 
**   (DDL, write DML, read DML) the function's return results vary.
**
**   The idea is to provide dynamic SQL support inside a UDR. I guess that
**   it's conceivable that you might want to put execute another SQL query
**   in a SQL query, but I find that a bit hard to fathom. Mind you, this will
**   support that, except for a couple of oddities (DROP TABLE)
**
**   IMPLEMENTATION NOTES:
**
**   The memory management on this is a little tricky, particularly on
**   the Iterator. I am executing a query on the first pass and then handing
**   back rows on each subsequent call. This means that the memory allocated
**   by the engine to hold query context -- type_desc etc -- must be 
**   allocated as PER_COMMAND memory. The only way to do this is to mi_switch
**   to PER_COMMAND before anything else. 
**
**   This means that I must be especially careful to release intermediate
**   results, and to ensure that return results are allocated as PER_ROUTINE
**   memory so that the engine cleans then up automatically. Checking this
**   requires a large wad of regression_tests specifically designed to 
**   see if the engine allocates additional chunks of memory.
**
**   USAGE NOTES:
**
**   You can't do a DROP in a UDR.
**
**   Using these, I can create any number of application-specific extensions,
**   and I have included a couple of SPL examples.
**
**   Exec_for_MSet ( lvarchar ) -> MSET( lvarchar NOT NULL )
**
*/
#include "exec.h"
/*
**
**   Support Routines.
**
**   Send_Query ( pszQuery, conn, pFparam )
**
**   Takes a query string and a connection. Submits the query. Returns an 
**   integer that represents what kind of query it was.
**
*/
mi_integer
Send_Query (
	mi_string	   * pszQuery,
	MI_CONNECTION   * conn,
	MI_FPARAM	   * pFparam
)
{
	mi_integer	nResult;

	if (mi_exec(conn,pszQuery,MI_QUERY_NORMAL) == MI_ERROR)
		mi_db_error_raise(conn, MI_EXCEPTION, "Exec: query failed");
		/* never reached */

	if ((nResult = mi_get_result(conn)) == MI_ERROR)
		mi_db_error_raise(conn, MI_EXCEPTION, "Exec: failed to return result");

	return nResult;
}
/*
**   Get_Next_Row( conn, pFparam )
**
**   Takes a connection with an active query on it. Returns either a string
**   that is the public format of a ROW type, or a NULL value (if there
**   are no more rows).
**
**   This function calls the exceedingly useful Get_Row_Data() routine that
**   takes a ROW and formats it as a lvarchar.
**
**/
mi_string *
Get_Next_Row (
	MI_CONNECTION   * conn,
	MI_FPARAM		* pFparam
)
{
	MI_ROW	  *	pszRowReturned;
	mi_string * pchRetVal;
	mi_integer  iMiError;
	   
	MI_MEMORY_DURATION nDuration;

	if ((pszRowReturned = mi_next_row(conn,&iMiError)) != NULL)
	{
		pchRetVal = Get_Row_Data( conn, pszRowReturned );
	} else {
		mi_query_finish(conn);
		pchRetVal = (mi_string *)NULL;
	}
	return pchRetVal;
}
/*
**   Get_Row_Data ( CONNECTION, ROW )
**
**   Takes a CONNECTION and a ROW and returns a string that is the public
**   format of an unnamed ROW TYPE corresponding to the result off the 
**   currently active query.
**
**   NOTE: In jagged row queries, the ROW TYPE varies.
**
*/
mi_string *
Get_Row_Data (
	MI_CONNECTION 	* conn,
	MI_ROW			* row
)
{
	mi_integer	  nNumFields, iColLen, i, 
				  nCurSize = MIN_BUF_SIZE, nLen = 0;
	MI_ROW_DESC  * pRowDesc;
	MI_DATUM	   datumRowColVal;
	MI_TYPEID	 * ptypeidColumn;
	MI_TYPE_DESC * ptypedescColumn;
	mi_string	 * pchRetBuf;
	mi_string	 * pstrRowString;
	mi_string	 * pchTypeName;
	mi_string	 * pchTmp;

	pchRetBuf  = (mi_string *)mi_zalloc(nCurSize);
	add_strings (&pchRetBuf, "ROW(", &nLen, &nCurSize);

	pRowDesc   = mi_get_row_desc ( row );
	nNumFields = mi_column_count ( pRowDesc );
	
	for (i=0;i<nNumFields;i++)
	{
		switch(mi_value(row, i, &datumRowColVal, &iColLen))
		{
			case MI_NULL_VALUE:
				pchTmp = mi_alloc(strlen("NULL,"));
				sprintf(pchTmp,"%s","NULL,");
				pstrRowString=pchTmp;
			 break;

/*
** Note: Recursive call here.  I need to append a ',' to keep matters
**       legit. Unfortunately, this adds a pass through a potentially
**       quite lengthy string.
**
*/
			case MI_ROW_VALUE:
				pstrRowString = Get_Row_Data ( conn, datumRowColVal );
				pchTmp = (mi_string *)mi_alloc(strlen(pstrRowString) + 1);
				sprintf(pchTmp,"%s,", pstrRowString);
				mi_free(pstrRowString);
				pstrRowString=pchTmp;
			break;

			case MI_COLLECTION_VALUE:
			case MI_NORMAL_VALUE:

/*
** Now, based on the type of the column, I need to figure out whether or not
** to quote it. If this is a number, or a DISTINCT type of a number, or a 
** COLLECTION, then there is no need to bracket the values in quote symbols. 
** For any other type, I ought to do so.
**
** Note: although it is perfectly reasonable to quote numbers in a SQL
**       expression, I spend a fair bit of effort here to avoid it. This
**       is purely aesthetic.
**/
				ptypeidColumn   = mi_column_type_id ( pRowDesc, i);
				ptypedescColumn = mi_type_typedesc  (conn, ptypeidColumn);

				pchTmp = (mi_string *)mi_zalloc(iColLen + 4); /*Size of result*/
                                                              /*plus space for*/
                                                              /*quotes & comma*/

				if (!( mi_typeid_is_complex( ptypeidColumn)))
				{
					if (mi_typeid_is_distinct(ptypeidColumn))
						ptypedescColumn=mi_get_type_source_type(ptypedescColumn);
					pchTypeName = mi_type_typename(ptypedescColumn);
	
					if (( 0 != (strcmp( pchTypeName, "integer"))) &&
					    ( 0 != (strcmp( pchTypeName, "int8"))) &&
					    ( 0 != (strcmp( pchTypeName, "decimal"))) &&
					    ( 0 != (strcmp( pchTypeName, "serial"))) &&
						( 0 != (strcmp( pchTypeName, "serial8"))) &&
						( 0 != (strcmp( pchTypeName, "float"))) &&
						( 0 != (strcmp( pchTypeName, "smallfloat"))) &&
						( 0 != (strcmp( pchTypeName, "smallint"))))
					{                                         /* Not a number */
						sprintf(pchTmp,"'%s',", datumRowColVal);
					} else {								  /* It's a number*/
						sprintf(pchTmp,"%s,", datumRowColVal);
					}
					mi_free( pchTypeName );
				} else {                                      /* Complex type*/
					if ((mi_typeid_is_collection(ptypeidColumn))==MI_TRUE)
					{
						sprintf(pchTmp,"%s,", datumRowColVal);
					} else {
						sprintf(pchTmp,"'%s',", datumRowColVal);
					}
				}
				mi_free( ptypeidColumn );    /* Although these are allocated */
				mi_free( ptypedescColumn );  /* by the ORDBMS, I haveset the */
				pstrRowString=pchTmp;        /* memory duration PER_COMMAND  */
                                             /* so I must free explicitly.   */
			 break;

			 default:
				mi_db_error_raise(conn, MI_EXCEPTION,
					"Exec: bad return result from mi_value()");
				 /* not reached */
			 break;
		}
		add_strings( &pchRetBuf, pstrRowString, &nLen, &nCurSize);
		mi_free(pstrRowString );
	}

	pchRetBuf[nLen-1]='\0';					 /* Trim last comma and add a */
	strcat(pchRetBuf,")");					 /* ')'.					  */
	return pchRetBuf;
}
/******************************************************************************
**
**   Function: Exec_to_String
**
**	  About:
**  
**	This function is the entry point for the SQL level function Exec().
**  
**/
mi_lvarchar *
Exec_to_String (
mi_lvarchar	 * plvQuery,
MI_FPARAM	   * pFparam
)
{
	MI_CONNECTION * conn;
	mi_lvarchar   * plvRetVal;
		
	conn = mi_open(NULL, NULL, NULL);

	if ((plvRetVal = Exec_to_String_with_Conn(conn,plvQuery,pFparam)) == NULL)
		plvRetVal =  mi_string_to_lvarchar("No Result");

	mi_close(conn);

	return plvRetVal;

}
/*
**   Function:   Exec_to_String_with_Conn()
**
**	  About:
**
**	This is the meat of the function above. If the query is a DDL or a 
**  write DML query, then the return result is a string describing what
**  happened. If the query is a SQL SELECT query, then the return result
**  is an lvarchar that is the public format of the ROW, from Get_Next_Row.
**
*/
mi_lvarchar *
Exec_to_String_with_Conn (
MI_CONNECTION   * conn, 
mi_lvarchar	 * plvQuery,
MI_FPARAM	   * pFparam
)
{
	mi_lvarchar	  * plvRetVal;
	MI_MEMORY_DURATION nDuration;
	mi_integer		 nQueryResult;
	mi_integer		 iNumCols;
	mi_string		* pchRetBuf;
	mi_string		 * pstrQuery;

	pstrQuery = mi_lvarchar_to_string(plvQuery);
 
	switch(Send_Query(pstrQuery, conn, pFparam))
	{
		default:
		case MI_ERROR:
			mi_db_error_raise(conn, MI_EXCEPTION, 
							  "Exec: failed to return result");
		 break;
 
		case MI_DML:
 
			iNumCols = mi_result_row_count( conn );
			pchRetBuf = (mi_string *)mi_alloc(26);
			sprintf(pchRetBuf, "%d Rows Affected", iNumCols);
 
		 break;
 
		case MI_DDL:
 
			pchRetBuf = (mi_string *)mi_alloc(10);
			sprintf(pchRetBuf,"%s","OK");
 
		 break;
 
		case MI_ROWS:

		 pchRetBuf = Get_Next_Row (conn, pFparam);

		break;
	}
	if (pchRetBuf == NULL)
		return (mi_lvarchar *)NULL;

	nDuration = mi_switch_mem_duration ( PER_ROUTINE ); 
	plvRetVal = mi_string_to_lvarchar(pchRetBuf);
	mi_switch_mem_duration ( nDuration ); 
	mi_free( pchRetBuf );
	
	return plvRetVal;
	
}
/********************************************************************
**
**   Function: Exec_Iterator
**
**	  About:
**
**/
mi_lvarchar *
Exec_Iterator (
mi_lvarchar	 * plvQuery,
MI_FPARAM	   * pFparam
)
{

	MI_CONNECTION * conn;
	mi_string	   * pstRetVal;
	mi_lvarchar   * plvValueFromFirstPass;
	mi_lvarchar   * plvTmpVal;
	mi_lvarchar   * plvRetVal;
	Conn_and_String * pConnandString;
	MI_MEMORY_DURATION nDuration;
	mi_string		 pchTemp[1024];
	
	switch(mi_fp_request(pFparam))
	{
		case SET_INIT:
		{
			mi_switch_mem_duration ( PER_COMMAND ); 

			pConnandString=(Conn_and_String *)mi_alloc(sizeof(Conn_and_String));
			mi_fp_setfuncstate(pFparam,pConnandString);

			conn = mi_open(NULL,NULL,NULL);	   /* Get the conn and set it  */
			pConnandString->conn=conn;			/* in the state being passed*/

/*
**  Call the Exec_to_String_with_Conn() logic to submit the query and return
**  the initial result. This convoluted logic is made necessary by the way
**  the SET_INIT/SET_RETONE/SET_END logic works. Results from SET_INIT are
**  ignored, but I can only send the query once and I need the Iterator
**  to behave sensibly when it is asked to do a DDL or a write DML too.
*/

			if ((plvValueFromFirstPass=Exec_to_String_with_Conn(conn, 
																plvQuery, 
																pFparam)) 
					== NULL)				  /* If the first pass result is  */
			{								 /* NULL, then the Exec_* logic  */
				return plvValueFromFirstPass; /* will have set the return	 */
			}								 /* using mi_fp_setreturnisnull()*/

													  /* Because the initial  */
			pConnandString->plvInterVal=			  /* result will not be   */
				  mi_var_copy(plvValueFromFirstPass); /* returned 'til next   */
													  /* (first SET_RETONE)   */
													  /* call, the memory used*/
													  /* to store it must be  */
													  /* PER_COMMAND.		 */
									   
			pstRetVal=mi_lvarchar_to_string(plvValueFromFirstPass);
														/* If what comes back */
			if(pstRetVal[0]!='R')					   /* is not a ROW, then */
			{										   /* I need to set the  */
				mi_close(pConnandString->conn);		 /* Inter call state to*/
				pConnandString->conn=0;				 /* look at only one   */
			}										   /* result row.		*/
			mi_free( pstRetVal );
			
		}
		break;

		case SET_RETONE:
/*
**  By this time, I have a live query and data to return. But each call gets
**  the next row, and returns the value that was retrieved the last time
**  it was called. 
**
**   To handle the end of the query results correctly, I end the query,
**  close the connection and set the value of the (MI_CONNECTION *) being
**  passed around to NULL. At this time I also return the row retrieved
**  in the *previous* pass. 
**
**   On the next SET_RETONE call, I check to see if the conn is NULL. If it
**  is, and there is still data to be returned (plvInterVal != NULL) then
**  first return the cached value before indicating that the last one is
**  gone.
**
*/
			mi_switch_mem_duration ( PER_COMMAND ); 
			pConnandString = (Conn_and_String *)mi_fp_funcstate(pFparam);

			if ( pConnandString->conn == NULL)			/* At the end?	  */
			{
				if ( pConnandString->plvInterVal == NULL) /* Yes. And no more */
				{
					mi_fp_setreturnisnull(pFparam, 0, MI_TRUE);
					mi_fp_setisdone(pFparam, MI_TRUE);
					plvRetVal=(mi_lvarchar *)NULL;
				} else {								  /* Yes. But one more*/
					nDuration = mi_switch_mem_duration ( PER_ROUTINE );
					plvRetVal=mi_var_copy(pConnandString->plvInterVal);
					mi_switch_mem_duration ( nDuration ); 

					mi_var_free(pConnandString->plvInterVal);
					pConnandString->plvInterVal = 0;	  /* Now, no more.	*/
				}
			} else {							/* Not at end. More rows to do*/
	
/*
** Get the next ROW from the query.
*/
				if (( pstRetVal = Get_Next_Row (pConnandString->conn, 
												pFparam)) == NULL )
				{										 /*No more from Q. So*/
					mi_close(pConnandString->conn);	   /*close the conn and*/
					pConnandString->conn=0;			   /*set the conn value*/

					plvTmpVal=mi_var_copy(pConnandString->plvInterVal);
					mi_var_free(pConnandString->plvInterVal);
					pConnandString->plvInterVal=0;

				} else { /* Return what you got on the previous pass and stow */
						 /* the current value.								*/

					plvTmpVal=mi_var_copy(pConnandString->plvInterVal); 
					mi_var_free(pConnandString->plvInterVal);

					pConnandString->plvInterVal=
									 mi_string_to_lvarchar(pstRetVal);
					mi_free( pstRetVal );
				}

				nDuration = mi_switch_mem_duration ( PER_ROUTINE );
				plvRetVal= mi_var_copy(plvTmpVal);
				mi_switch_mem_duration ( nDuration ); 
				mi_var_free( plvTmpVal );
			}
			
			return plvRetVal;

		 break;

		case SET_END:
			mi_switch_mem_duration ( PER_ROUTINE ); 
			mi_fp_setreturnisnull(pFparam, 0, MI_TRUE);
			return (mi_lvarchar *)NULL;
		 break;

	default:
		mi_db_error_raise(NULL, MI_EXCEPTION, "Exec_for_Row: Bad iterator");
	 break;
	}
}
/*
**  add_strings ( mi_string ** To, mi_string * From, mi_integer ToLen, 
**				mi_integer ToMax )
**
**
**  Ensures that, when concatenating two string, you don't run out of memory
** in the assigned space.
**	  
*/
void add_strings (
 mi_string	**	pchTo,
 mi_string   *  pchFrom,
 mi_integer  *  nToLen,
 mi_integer  *  nToMaxLen
)
{
	mi_string	*	pchInternalTemp;
	mi_integer		nLenFrom;

	nLenFrom = strlen(pchFrom);
	if ((*nToMaxLen) <= ((*nToLen) + nLenFrom + 1))
	{
		pchInternalTemp = (mi_string *)mi_alloc((*nToMaxLen) + nLenFrom 
											     + MIN_BUF_SIZE);
		
		(*nToMaxLen) = (((*nToMaxLen) + nLenFrom + MIN_BUF_SIZE));
		strcpy(pchInternalTemp,(*pchTo));
		mi_free((*pchTo));
		(*pchTo) = pchInternalTemp;
	}

	strcat((*pchTo), pchFrom);
	(*nToLen)+=nLenFrom;

}
