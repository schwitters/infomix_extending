/*
 *  File:  exec.h
 *
 *    By:  Paul Brown
 *
 * About:
 *
 */
#include <stdio.h>
#include <mi.h>

#define MIN_BUF_SIZE 12

typedef struct {
	MI_CONNECTION * conn;
	mi_lvarchar   * plvInterVal;
} Conn_and_String;

void add_strings ( mi_string **, mi_string *, mi_integer *, mi_integer *);

mi_integer Send_Query ( mi_string * , MI_CONNECTION *,MI_FPARAM * );
mi_string * Get_Next_Row ( MI_CONNECTION *, MI_FPARAM * );
mi_string * Get_Row_Data ( MI_CONNECTION *, MI_ROW * );

mi_lvarchar * Exec_to_String ( mi_lvarchar *, MI_FPARAM * );
mi_lvarchar * Exec_to_String_with_Conn ( MI_CONNECTION *, mi_lvarchar *, 
                                         MI_FPARAM * );
mi_lvarchar * Exec_Iterator  ( mi_lvarchar *, MI_FPARAM * );

