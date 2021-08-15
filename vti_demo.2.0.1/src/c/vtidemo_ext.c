/* ****************************************************************************
** vtidemo_ext.c
**
** Description: External data manager for the vti demo.
**
**              This stand-alone program handles all interaction with the
**              external data store used in the vti demo. It reads requests
**              posted by VTI purpose functions on a "well known" FIFO, and
**		returns data or status information.
**
** Parameters:  
**
**              Input: (none)
**
**              Out:   (writes an audit trail to stderr)
**
** ****************************************************************************
*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "vtidemo_ext.h"

int
main ()
{
    char   inbuf[BUFSIZ];    /* buffer for string received from client */
    char   outbuf[BUFSIZ];   /* buffer for string sent by server   */
    int    n;                /* bytes received from the client     */
    int    i, j, k;          /* counters                           */
    int    rslt;             /* function call results              */
    int    server_fd;        /* file handle for the server's FIFO  */
    char   server_fifo[BUFSIZ];      /* file name for the server's FIFO    */
    int    client_fd;        /* file handle for the client's FIFO  */
    char   client_fifo[BUFSIZ];      /* file name for the client's FIFO    */
    int    retval;           /* called routine return value        */
    char   trans_buf[3];     /* transaction flag as char string    */
    char   data_buf[BUFSIZ]; /* follow-up data for multi-part transaction  */

    typedef enum trans_types
    {
        CREATE_TABLE = 1,
        DROP_TABLE,    /* 2 */
        INSERT,        /* 3 */
        UPDATE,        /* 4 */
        GETBYID,       /* 5 */
        DELETE,        /* 6 */
        BEGINSCAN,     /* 7 */
        ENDSCAN,       /* 8 */
        GETSTATS,      /* 9 */
        SCANCOST,      /* 10 */
        CHECK          /* 11 */
    } trans_type;

    trans_type trans_flag;   /* transaction flag                   */

    /* external support routines */
    extern int create_table();
    extern int drop_table();

    (void)fprintf(stderr, "\nExternal data server starting...\n");

    /* ***********************************************************************
    ** Create the data server's fifo. This is the "well-known address" 
    ** that clients (vti purpose functions) will write to.            
    ** ***********************************************************************
    */
    (void)sprintf(server_fifo, "/tmp/idn_ext_fifo.tmp");
    rslt = mkfifo(server_fifo, (mode_t) 0666);
    if (rslt < 0)
    {
        if (errno == EEXIST)
        {
            /* go ahead and unlink it, then try again */
            (void)unlink(server_fifo);
            if ((rslt = mkfifo(server_fifo, (mode_t)0666)) < 0)
            {
              (void)fprintf(stderr, "\nmkfifo() failed for external FIFO!\n");
              exit(1);
            }
        }
    }

    /* Open it. */
    if ((server_fd = open(server_fifo, O_RDWR, 0666)) < 0)
    {
        (void)fprintf(stderr, "\nUnable to open() server FIFO.\n");
        (void)fprintf(stderr, "errno is: %d\n", errno);
        exit(1);
    }
 
    /* Read forever */
    while (1)
    {
        memset(inbuf, 0, BUFSIZ);
        (void)fprintf(stderr, "Reading...\n");
        n = read(server_fd, inbuf, (size_t)BUFSIZ);

        /* ********************************************************************
        ** Transactions consist of:
        ** 
        **    (1) client identifier (fifo name), 
        **    (2) transaction code, and 
        **    (3) transaction text.
        ** 
        ** Every transaction completes with a status report ("OK" or "ERROR")
        ** sent back on the client's fifo.
        **
        ** Some transaction types involve an additional exchange, which is 
        ** handled within the code for that transaction.
        ** ********************************************************************
        */
        (void)fprintf(stderr, "Got a transaction: %s\n", inbuf);

        /* Get the client fifo name and open it */
        i = 0;
        while (( i < n) && (inbuf[i] != ' '))
        {
            client_fifo[i] = inbuf[i];
            i++;
        }

        if (i > n)
        {
            (void)fprintf(stderr, "Unable to extract client fifo name.\n");
            strcpy(outbuf, "ERROR");
            goto send_response;
        }

        client_fifo[i] = '\0';
        (void)fprintf(stderr, "Client FIFO is: %s\n", client_fifo);

        /* Open the client FIFO */
        if ((client_fd = open(client_fifo, O_RDWR, 0666)) < 0)
        {
            (void)fprintf(stderr, "\nUnable to open() client FIFO.\n");
            (void)fprintf(stderr, "errno is: %d\n", errno);
            strcpy(outbuf, "ERROR");
            goto send_response;
        }

        i++; /* skip past the space */

        /* Get the transaction code */
        j = 0;
        while (( i < n) && (inbuf[i] != ' '))
        {
            trans_buf[j] = inbuf[i];
            i++;
            j++;
        }
 
        if (i >= n)
        {
            (void)fprintf(stderr, "Unable to extract transaction code.\n");
            strcpy(outbuf, "ERROR");
            goto send_response;
        }

        trans_buf[j] = '\0';
        i++; /* skip past the space */

        trans_flag = atoi(trans_buf);
        if (trans_flag == 0)
        {
            (void)fprintf(stderr, "Bad transaction code: %s\n", trans_buf);
            strcpy(outbuf, "ERROR");
            goto send_response;
        }

        /* Get the rest of the transaction. This will vary depending  */
        /* on what kind of transaction we have.                       */
        k = 0;
        while ( i < n)
        {
            data_buf[k] = inbuf[i];
            i++;
            k++;
        }
        data_buf[k] = '\0';

        /* initialize the return buffer and process the transaction */
        strcpy(outbuf, "OK");

        retval = 0;
        switch (trans_flag)
        {
          case CREATE_TABLE:
            /* create a new table */
            retval = create_table(data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case DROP_TABLE:
            /* drop an existing table */
            retval = drop_table(data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case INSERT:
            /* insert a row */
            retval = insert_row(server_fd, client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case UPDATE:
            /* update a row */
            retval = update_row(client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case GETBYID:
            /* retrieve a specific row */
            retval = get_byid(client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case DELETE:
            /* delete a row */
            retval = delete_row(client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case BEGINSCAN:
            /* parse the qualification and write qualifying rows to the
            ** client's fifo
            */
            retval = begin_scan(client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;
    
          case ENDSCAN:
            /* nothing needed here - the udr handles everything */
            retval = 0;
            break;

          case GETSTATS:
            /* get info for updating tstats */
            retval = get_stats(client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          case SCANCOST:
            /* get info for the optimizer */
            retval = 0;
            break;

          case CHECK:
            /* (minimal) data integrity check */
            retval = check_file(client_fd, data_buf, outbuf);
            if (retval < 0)
            {
                strcpy(outbuf, "ERROR");
            }
            break;

          default:
            /* bad transaction type. log it and report back */
            (void)fprintf(stderr, "Bad transaction code: %s\n", trans_buf);
            retval = -1;
            strcpy(outbuf, "ERROR");
            break;
        }

send_response:

        (void)fprintf(stderr, "Transaction complete. Sending back: %s\n\n",
                      outbuf);

        n = strlen(outbuf) + 1;
        rslt = write(client_fd, outbuf, n);
        if (rslt != n)
        {
          (void)fprintf(stderr, 
                "Error: tried to write %d bytes, but wrote %d\n", n, rslt);
        }

        close(client_fd);

    } /* while */
    /* not reached */
    return 0;
}

/* ***************************************************************************
** support routines 
** *************************************************************************** 
*/
int
create_table(char *inbuf, char *outbuf)
{
    int     tab_fd;

    (void)fprintf(stderr, "File name for new table is %s\n", inbuf);
    tab_fd = creat(inbuf, S_IRWXU|S_IRWXG|S_IRWXO);

    if (tab_fd < 0)
    {
      (void)fprintf(stderr, "Error creating table file: %s\n", inbuf);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      strcpy(outbuf, "ERROR");
      return -1;
    }
    
    strcpy(outbuf, "OK");
    close(tab_fd);
    return 0;
}

int
drop_table(char *inbuf, char *outbuf)
{
    if (unlink(inbuf) < 0)
    {
      /* don't abort, just report that the file wasn't removed */
      (void)fprintf(stderr, "Table file %s was not removed.\n", inbuf);
      (void)fprintf(stderr, "errno is: %d\n", errno);
    }

    strcpy(outbuf, "OK");
    return 0;
}

int
insert_row(int server_fd, int client_fd, char *inbuf, 
           char *outbuf)
{
    char    pathname[BUFSIZ];  /* path for the file holding the table  */
    int     fd;                /* handle for file holding the table    */
    int     rowid;             /* id for the new row                   */
    int     rowlen;            /* row length, to calculate new rowid   */
    char    rowdata[BUFSIZ];   /* raw row data                         */
    int     fsize;             /* size of the file at pathname         */
    int     n;

    /* get the row length and file path */
    if (sscanf(inbuf, "%d %s", &rowlen, pathname) != 2)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    /* **********************************************************************
    ** get the next rowid and send it back -- the server needs this to
    ** complete the transaction. this will be the signal for the UDR to
    ** send the row data.
    ** **********************************************************************
    */

    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_APPEND|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* get the next rowid */
    if ((fsize = lseek(fd, 0, SEEK_END)) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error seeking to end of file.\n");
      return -1;
    }
    rowid = fsize / rowlen + 1;

    /* send it back */
    n = write(client_fd, (void *)&rowid, sizeof(rowid));   

    /* **********************************************************************
    ** now read back the row data and write it to the file.
    ** if the UDR was unable to read the rowid, it will have sent back
    ** the string "ERROR" as a signal for us to bail out.
    ** **********************************************************************
    */
    n = read(server_fd, rowdata, rowlen);
    if ((n != rowlen) || (strcmp(rowdata, "ERROR") == 0))
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error reading row data from UDR\n");
      return -1;
    }

    n = write(fd, rowdata, rowlen);
    if (n != rowlen)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error: tried to write %d bytes, but wrote %d\n",
            rowlen, n);
      return -1;
    }

    /* ok, we're done here. */
    close(fd);
    return 0;
}

int
update_row(int client_fd, char *inbuf, char *outbuf)
{
    int     fd;                /* handle for file holding the table    */
    int     rowlen;
    int     rowid;
    char    pathname[BUFSIZ];
    char    rowbuf[500];
    int     i, j;

    /* get the pathname and rowid from the buffer */
    if (sscanf(inbuf, "%d %d %s", &rowlen, &rowid, pathname) != 3)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    /* position to the new row data in inbuf */
    i = j = 0;
    while (j < 3)
    {
      if (inbuf[i] == ' ')
      {
        j++;
      }
      i++;
    }
    inbuf += i;

    (void)fprintf(stderr, "row to be updated is: %d, pathname is %s\n", rowid,
          pathname);

    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* seek to the requested row */
    if (lseek(fd, (rowid - 1) * rowlen, SEEK_SET) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error seeking to record to be updated.\n");
      return -1;
    }

    /* and overwrite it with the new row */
    if (write(fd, inbuf, rowlen) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error rewriting row.\n");
      return -1;
    }

    close(fd);
    strcpy(outbuf, "OK");
    return 0;
}

int
get_byid(int client_fd, char *inbuf, char *outbuf)
{
    int     fd;                /* handle for file holding the table    */
    int     rowlen;
    int     rowid;
    char    pathname[BUFSIZ];
    char    rowbuf[500];

    /* get rowlength, rowid, and table pathname from the buffer */
    if (sscanf(inbuf, "%d %d %s", &rowlen, &rowid, pathname) != 3)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    (void)fprintf(stderr, "Fetching row %d by rowid.\n", rowid);

    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* seek to the requested row */
    if (lseek(fd, (rowid - 1) * rowlen, SEEK_SET) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error seeking to desired row.\n");
      return -1;
    }

    /* get the current record */
    if (read(fd, (void *)rowbuf, (size_t)rowlen) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error reading record prior to update.\n");
      return -1;
    }

    /* check the "deleted" flag */
    if (rowbuf[0] == 'D')
    {
      close(fd);
      strcpy(outbuf, "OK"); /* meaning: we got the row but it's deleted */
      return 0;
    }

    /* put the row in the return buffer */
    memcpy(outbuf, rowbuf, rowlen);

    close(fd);
    return 0;
}
int
delete_row(int client_fd, char *inbuf, char *outbuf)
{
    int     fd;                /* handle for file holding the table    */
    int     rowlen;
    int     rowid;
    char    pathname[BUFSIZ];
    char    rowbuf[500];

    /* get the pathname and rowid from the buffer */
    if (sscanf(inbuf, "%d %d %s", &rowlen, &rowid, pathname) != 3)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    (void)fprintf(stderr, "row to be deleted is: %d, pathname is %s\n", rowid,
          pathname);


    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* seek to the requested row */
    if (lseek(fd, (rowid - 1) * rowlen, SEEK_SET) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error seeking to record to be deleted.\n");
      return -1;
    }

    /* get the current record */
    if (read(fd, (void *)rowbuf, (size_t)rowlen) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error reading record prior to update.\n");
      return -1;
    }

    /* set the "deleted" flag */
    rowbuf[0] = 'D';

    /* seek back to the beginning of the record and rewrite it */
    if (lseek(fd, (rowid - 1) * rowlen, SEEK_SET) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error seeking back to record to be deleted.\n");
      return -1;
    }

    if (write(fd, rowbuf, rowlen) < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error rewriting row.\n");
      return -1;
    }

    close(fd);
    strcpy(outbuf, "OK");
    return 0;
}

/* ***************************************************************************
** begin_scan()
**
** This routine does all the work involved in carrying out a scan. It opens
** a new FIFO for the exclusive use of the session initiating the scan, and
** sends its name back to the UDR.
**
** The UDR then sends a copy of the qualification descriptor. This routine
** scans through the data in the table, comparing rows against the QD, and
** writing any qualifying rows to the new FIFO. The rows will be read later
** by the access method's getnext() routine.
** ***************************************************************************
*/
int
begin_scan(int client_fd, char *data_buf, char *outbuf)
{
    int         rowlen;              /* length of a row                     */
    int         ncols;               /* number of columns in a row          */
    char        coltypes[11];        /* types of the columns                */
    int         collength[10];       /* size of each column                 */
    char        nulls[11];           /* null indicator for each column      */
    char        pathname[BUFSIZ];    /* location of the table's file        */
    int         n, i;                /* counters                            */
    int         fd;                  /* data table file handle              */
    char        data_fifo[BUFSIZ];   /* pathname for data fifo              */
    int         data_fifo_fd;        /* fd for data fifo                    */
    char        buff[BUFSIZ];        /* generic buffer to read into         */
    vtidemo_qual_t qual;             /* our own qualification structure     */
    outrec_t    outrec;              /* record to return to udr             */
    int         rslt;                /* function return result              */

    extern void debug_print_qual(vtidemo_qual_t *, char *);

    /* get the table information */

    if (sscanf(data_buf, "%d %d %d %d %d %d %d %d %d %d %d %d %s %s %s", 
        &rowlen, &ncols, &collength[0], &collength[1], &collength[2],
        &collength[3], &collength[4], &collength[5], &collength[6], 
        &collength[7], &collength[8], &collength[9],
        nulls, coltypes, pathname) != 15)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    /* *********************************************************************
    ** open the new fifo and send its name back to the UDR. the UDR will
    ** use this to send us the qualification buffer, then we'll use it to
    ** store all the rows that qualify.                                   
    ** *********************************************************************
    */
    sprintf(data_fifo, "/tmp/vtidemo_data-%d", time((time_t)0));
    if (mkfifo(data_fifo, (mode_t) 0666) < 0)
    {
        (void)fprintf(stderr, "Unable to create data fifo: %s\n", data_fifo);
        strcpy(outbuf, "ERROR");
        return -1;
    }

    data_fifo_fd = open(data_fifo, O_RDWR, 0666);
    if (data_fifo_fd < 0)
    {
        (void)fprintf(stderr, "Unable to open data fifo: %s\n", data_fifo);
        strcpy(outbuf, "ERROR");
        return -1;
    }

    n = strlen(data_fifo) + 1;
    write(client_fd, data_fifo, n);

    /* **********************************************************************
    ** give the UDR a moment to respond, then read the qualification 
    ** text from the new fifo. we use this fifo to avoid potential
    ** confusion with messages from other clients. 
    ** **********************************************************************
    */
    (void)sleep(1);
    memset(buff, 0, BUFSIZ);
    n = read(data_fifo_fd, buff, BUFSIZ);
    if (n < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, 
            "Error trying to receive qualification descriptor\n");
      return -1;
    }

    (void)fprintf(stderr, 
          "Received qualification buffer containing %d bytes:\n", n);
    (void)fprintf(stderr, "%s\n", buff);

    if (demo_parse_qual(buff, coltypes, &qual) < 0)
    {
      (void)fprintf(stderr, "Error from demo_parse_qual().\n");
      return -1;
    }

    (void)fprintf(stderr, "\nParsed qualification tree contains:\n");
    debug_print_qual(&qual, "");
    (void)fprintf(stderr, "\n");

    /* write qualifying rows to the new fifo */
    /* ***********************************************************************
    ** TIP: For a performance improvement, consider doing the work
    **      of qualifying rows and writing them to the new fifo in
    **      a separate thread or a forked process.
    ** ***********************************************************************
    */
    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* get rows */
    i = 0;
    while (read(fd, (void *)outrec.rowbuf, (size_t)rowlen) > 0)
    {
      i++; /* to keep track of rowids */
      if (outrec.rowbuf[0] != 'D') /* skip deleted rows */
      {

        /* here is where we qualify rows */
        if (qual.qual_isnull == 1)
        {
          /* null qualification descriptor: all rows qualify. */
          outrec.rowqualified = 1;
          rslt = 1;
        }
        else
        {
          outrec.rowqualified = demo_qualify_row(outrec.rowbuf, &qual, 
                                coltypes, collength);
        }

        if ((outrec.rowqualified == 1) || (outrec.rowqualified == -1))
        {
          /* return any rows that either qualify or that we can't resolve */
          outrec.rowid = i;
          if (write(data_fifo_fd, (void *)&outrec, 
             (size_t)rowlen + sizeof(outrec.rowid) 
                            + sizeof(outrec.rowqualified)) < 0)
          {
            close(fd);
            close(data_fifo_fd);
            strcpy(outbuf, "ERROR");
            (void)fprintf(stderr, "Error writing row to data fifo file %s\n",
                          data_fifo);
            return -1;
          }
        } /* if row qualifies */
      } /* if not deleted... */
    } /* while read... */
    
    strcpy(outbuf, "OK");
    close(data_fifo_fd);
    close(fd);

    return 0;
}

/* ***************************************************************************
** check_file()
**
** Just a minimal check for file integrity -- verify that the file size is an
** even multiple of the row size. A real "am_check" routine would be much more
** rigorous!
** ***************************************************************************
*/
int
check_file(int client_fd, char *inbuf, char *outbuf)
{
    int     fd;                /* handle for file holding the table    */
    int     rowlen;
    char    pathname[BUFSIZ];
    int     fsize;
    int     numrows;

    /* get the pathname and rowid from the buffer */
    if (sscanf(inbuf, "%d %s", &rowlen, pathname) != 2)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* get the number of rows */
    fsize = lseek(fd, 0, SEEK_END);
    if (fsize < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error getting size of table file\n");
      return -1;
    }

    if ((fsize%rowlen) != 0)
    {
      strcpy(outbuf, "ERROR");
    }

    close(fd);
    return 0;
}

/* ***************************************************************************
** get_stats()
**
** Return the number of rows in the table. We probably shouldn't count the
** rows that are flagged as deleted, but since they're still physically
** present, we do count them.
** ***************************************************************************
*/
int
get_stats(int client_fd, char *inbuf, char *outbuf)
{
    int     fd;                /* handle for file holding the table    */
    int     rowlen;
    char    pathname[BUFSIZ];
    int     fsize;
    int     numrows;

    /* get the pathname and rowid from the buffer */
    if (sscanf(inbuf, "%d %s", &rowlen, pathname) != 2)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error parsing input buffer.\n");
      return -1;
    }

    /* open the file holding the table data */
    if ((fd = open(pathname, O_RDWR|O_SYNC)) < 0)
    {
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error opening file: %s\n", pathname);
      (void)fprintf(stderr, "errno is: %d\n", errno);
      return -1;
    }

    /* get the number of rows */
    fsize = lseek(fd, 0, SEEK_END);
    if (fsize < 0)
    {
      close(fd);
      strcpy(outbuf, "ERROR");
      (void)fprintf(stderr, "Error getting size of table file\n");
      return -1;
    }

    numrows = fsize / rowlen;
    sprintf(outbuf, "%d", numrows);

    close(fd);
    return 0;
}

/* ***************************************************************************
** demo_parse_qual()
**
** Build a structure similar to an MI_AM_QUAL_DESC. We'll use this to evaluate
** each row.
** ***************************************************************************
*/
int
demo_parse_qual(char *qualbuf, char *coltypes, vtidemo_qual_t *qual)
{
    char    *qualbuf_ptr = qualbuf; /* shrinking qualbuf for iteration      */
    int     i;                      /* counter                              */
    int     q_bytes;                /* another counter                      */
    char    q_word[500];            /* max row size for demo is 500         */
    int     rslt;                   /* return value                         */
   
    /* ***********************************************************************
    ** Read ahead to the first alphabetic character. A brand-new qualification
    ** buffer will always start with one, but we're going to call this routine
    ** reiteratively. When we re-enter we'll be somewhere in the middle of the
    ** qualification buffer, so we need to scan forward to the first word.
    ** ***********************************************************************
    */
    q_bytes = 0;
    while ((q_bytes < strlen(qualbuf)) && ( ! isalpha(qualbuf[q_bytes])))
    {
      q_bytes++;
    }
    if (q_bytes > strlen(qualbuf))
    {
      (void)fprintf(stderr, "Unable to read qualification buffer.\n");
      return -1;
    }

    /* **********************************************************************
    ** Get the first word. It will either be "null", "and", "or", "not", or a
    ** function name for a simple predicate.
    ** **********************************************************************
    */
    i = 0;
    while ((q_bytes < strlen(qualbuf)) && (qualbuf[q_bytes] != ' '))
    {
      q_word[i] = qualbuf[q_bytes];
      q_bytes++;
      i++;
    }
    if (q_bytes > strlen(qualbuf))
    {
      (void)fprintf(stderr, "Unable to read qualification buffer.\n");
      return -1;
    }

    q_word[i] = '\0';

    if (strncmp(q_word, "null", 4) == 0)
    {
      /* qd is null. we're done here */
      qual->qual_isnull = 1;
      rslt = 0;
    }
    else
    {
      /* not null. is it a complex qualification or a simple predicate? */
      qual->qual_isnull = 0;
      if ((! strncmp(q_word, "or", 2)) || (! strncmp(q_word, "and", 3)))
      {
        /* complex qualification */
        rslt = demo_process_complex(qualbuf_ptr, qual, coltypes);
        if (rslt < 0)
        {
          (void)fprintf(stderr, "Failure processing complex qualification.\n");
        }
      }
      else
      {
        /* simple predicate */
        rslt = demo_process_simple(qualbuf_ptr, qual, coltypes);
        if (rslt < 0)
        {
          (void)fprintf(stderr, "Error processing simple qualification.\n");
        }
      }
    }
    return rslt;
}

/* ***************************************************************************
** demo_process_complex()
**
** Take care of all the processing for a complex qualification.
** ***************************************************************************
*/
int
demo_process_complex(char *qualbuf_ptr, vtidemo_qual_t *qual, char *coltypes)
{

    int         q_bytes;
    char        q_word[5];      /* "and" or "or" */
    int         num_subquals;
    int         i;
    int         rslt;
    int         tot_rslt = 0;

    memset(q_word, 0, 5);
    /* skip any white space */
    q_bytes = 0;
    while ((q_bytes < strlen(qualbuf_ptr)) && (! isalpha(qualbuf_ptr[q_bytes])))
    {
      q_bytes++;
    }
    if (q_bytes > strlen(qualbuf_ptr))
    {
      (void)fprintf(stderr, "Unable to read qualification buffer.\n");
      return -1;
    }

    /* Get the boolean operator */
    i = 0;
    while ((q_bytes < strlen(qualbuf_ptr)) && (qualbuf_ptr[q_bytes] != ' ')
          && (i < 5))
    {
      q_word[i] = qualbuf_ptr[q_bytes];
      q_bytes++;
      i++;
    }
    if ((q_bytes > strlen(qualbuf_ptr)) || (i >= 5))
    {
      (void)fprintf(stderr, "Unable to get boolean operator.\n");
      return -1;
    }
    q_word[i] = '\0';

    /* set the issimple flag to false */
    qual->qual_issimple = 0;

    /* set the boolop flag to 1 ("and") or 2 ("or") */
    if (strncmp(q_word, "or", 2) == 0)
    {
      /* complex qualification - "or" */
      qual->qual_boolop = 2;
    }
    else
    {
      /* complex qualification - "and" */
      qual->qual_boolop = 1;
    }
   
    /* how many sub-qualifications do we have? */
    num_subquals = demo_count_subquals(qualbuf_ptr);
    if (num_subquals < 0)
    {
      (void)fprintf(stderr, 
            "Unable to get count of sub-quals for complex qual.\n");
      return -1;
    }
    qual->qual_nquals = num_subquals;

    /* allocate space for them */
    qual->qual_quals = 
        (vtidemo_qual_t *)malloc(sizeof(vtidemo_qual_t) * 
         num_subquals);

    /* skip past the boolean operator, then process the sub-qualifications */
    qualbuf_ptr += q_bytes;

    for (i=0; i<num_subquals; i++)
    {
      /* *************************************************************************
      ** demo_parse_qual() will return a count of the number of bytes 
      ** it consumed from the qual buffer. each recursive call picks 
      ** up the buffer from that point.                             
      ** *************************************************************************
      */
      rslt = demo_parse_qual(qualbuf_ptr, coltypes, &qual->qual_quals[i]);
      if (rslt < 0)
      {
        (void)fprintf(stderr, 
           "Error in recursive call to demo_parse_qual().\n");
        return rslt;
      }
      qualbuf_ptr += rslt;
      tot_rslt += rslt;
    }
    return tot_rslt;
}
        
/* ****************************************************************************
** demo_process_simple()
**
** Take care of the processing for a simple predicate.
** ****************************************************************************
*/
int
demo_process_simple(char *qualbuf_ptr, vtidemo_qual_t *qual, char *coltypes)
{
    int          i = 0;
    int          q_bytes = 0;
    char         q_word[50];

    qual->qual_issimple = 1;

    /* skip to the first alpha character */
    while ((q_bytes < strlen(qualbuf_ptr)) && (! isalpha(qualbuf_ptr[q_bytes])))
    {
      q_bytes++;
    }
    if (q_bytes >= strlen(qualbuf_ptr))
    {
      (void)fprintf(stderr, 
            "Error skipping to first alpha in demo_process_simple()\n");
      return -1;
    }
    
    /* get the first word. it will either be "not" or a func name */
    i = 0;
    while ((q_bytes < strlen(qualbuf_ptr)) && (qualbuf_ptr[q_bytes] != ' '))
    {
      q_word[i] = qualbuf_ptr[q_bytes];
      q_bytes++;
      i++;
    }
    if (q_bytes >= strlen(qualbuf_ptr))
    {
      (void)fprintf(stderr, 
            "Error getting first word in demo_process_simple()\n");
      return -1;
    }
    q_word[i] = '\0';

    if (! strncmp(q_word, "not", 3))
    {
      /* set the negate flag, then get the function name */
      qual->qual_negate = 1;

      /* skip the blank space */
      q_bytes++;

      /* get the function name */
      i = 0;
      while ((q_bytes < strlen(qualbuf_ptr)) && (qualbuf_ptr[q_bytes] != ' '))
      {
        q_word[i] = qualbuf_ptr[q_bytes];
        i++;
        q_bytes++;
      }
      if (q_bytes >= strlen(qualbuf_ptr))
      {
        (void)fprintf(stderr, 
              "Error getting func name for simple predicate.\n");
        return -1;
      }
      q_word[i] = '\0';
    }
    else
    {
      qual->qual_negate = 0;
    }

    /* *******************************************************************
    ** ok, now q_word holds the function name. just get the column number
    ** and the constant value and we're done.
    ** *******************************************************************
    */
    qual->qual_funcname = (char *)malloc(strlen(q_word) + 1);
    strcpy(qual->qual_funcname, q_word);
    /* read to the open paren */
    while ((qualbuf_ptr[q_bytes] != '(') && (i < strlen(qualbuf_ptr)))
    {
      q_bytes++;
    }
    if (q_bytes > strlen(qualbuf_ptr))
    {
      /* oops. qual buffer must have been mangled. bail out. */
      (void)fprintf(stderr, 
            "Unable to get simple predicate arguments. Exiting.\n");
      return -1;
    }

    /* now get the column number and type */
    q_bytes++;
    i = 0;
    while ((qualbuf_ptr[q_bytes] != ',') && (q_bytes < strlen(qualbuf_ptr)))
    {
      q_word[i] = qualbuf_ptr[q_bytes];
      q_bytes++;
      i++;
    }
    if (q_bytes > strlen(qualbuf_ptr))
    {
      /* oops. bail out. */
      (void)fprintf(stderr, 
            "Unable to get column number for simple predicate. Exiting.\n");
      return -1;
    }
    q_word[i] = '\0';
    qual->qual_column = atoi(q_word);
    qual->qual_coltype = coltypes[qual->qual_column] == 'i' ? 1 : 2;

    /* get the constant value */
    i = 0;
    /* skip to the opening quote */
    while ((qualbuf_ptr[q_bytes] != '"') && (q_bytes < strlen(qualbuf_ptr)))
    {
      q_bytes++;
    }
    if (q_bytes > strlen(qualbuf_ptr))
    {
      /* oops. bail out. */
      (void)fprintf(stderr, 
          "Unable to get constant value for simple predicate. Exiting.\n");
      return -1;
    }
    q_bytes++;
    /* and read characters until we reach the closing quote */
    while ((qualbuf_ptr[q_bytes] != '"') && (q_bytes < strlen(qualbuf_ptr)))
    {
      q_word[i] = qualbuf_ptr[q_bytes];
      q_bytes++;
      i++;
    }
    if (q_bytes > strlen(qualbuf_ptr))
    {
      /* oops. bail out. */
      (void)fprintf(stderr, 
            "Unable to get constant value for predicate. Exiting.\n");
      return -1;
    }
    q_word[i] = '\0';

    qual->qual_constant = (char *)malloc(strlen(q_word));
    strcpy(qual->qual_constant, q_word);

    /* and we're done! return a count of the number of bytes we processed */
    return q_bytes;
}

/* ***************************************************************************
** demo_count_subquals() 
**
** Count the number of subqualifications in a complex qualification. We do 
** this by navigating to the first left paren, then counting the number of
** commas we get until we have the corresponding right paren. Commas that
** are inside other pairs of parens don't count. The total number of commas
** plus one = the number of sub-qualifications.
** ***************************************************************************
*/
int
demo_count_subquals(char *qualbuf)
{
    int      lparens = 0;       /* ongoing count of left parens             */
    int      commas = 0;        /* ongoing count of commas                  */
    int      i = 0;             /* loop counter                             */

    /* Navigate to the left paren */
    while ((qualbuf[i] != '(') && (i <= strlen(qualbuf)))
    {
      i++;
    }
    if (i > strlen(qualbuf))
    {
      (void)fprintf(stderr, 
            "Error seeking to first left paren in demo_count_subquals.\n");
      return -1;
    }

    i++; /* skip past the paren */

    while ((i <= strlen(qualbuf) && (lparens >= 0)))
    {
      switch(qualbuf[i])
      {
        case '(' :  lparens++;
                    break;

        case ')' :  lparens--;
                    break;

        case ',' :  if (lparens == 0)
                    {
                      commas++;
                    }
                    break;

        default  :  /* do nothing */
                    break;
      }
      i++;
    }
    return ++commas;
}


/* ****************************************************************************
** debug_print_qual()
**
** for debugging purposes, print the contents of the qualification tree.
** ****************************************************************************
*/
void
debug_print_qual(vtidemo_qual_t *qual, char *indent_in)
{
  int   i;                  /* loop counter               */
  char  indent[100];        /* for indenting nested loops */

  sprintf(indent, "  %s", indent_in);

  (void)fprintf(stderr, "\n%squal_isnull   : %d\n", indent, 
                           qual->qual_isnull);
  if ( ! qual->qual_isnull)
  {
    (void)fprintf(stderr, "%squal_issimple : %d\n", indent, 
                           qual->qual_issimple);
    if (qual->qual_issimple)
    {
      (void)fprintf(stderr, "%squal_negate   : %d\n", indent, 
                             qual->qual_negate);
      (void)fprintf(stderr, "%squal_funcname : %s\n", indent, 
                             qual->qual_funcname);
      (void)fprintf(stderr, "%squal_column   : %d\n", indent, 
                             qual->qual_column);
      (void)fprintf(stderr, "%squal_coltype  : %d\n", indent, 
                             qual->qual_coltype);
      (void)fprintf(stderr, "%squal_constant : %s\n", indent, 
                             qual->qual_constant);
    }
    else
    {
      if (qual->qual_boolop == 1)
      {
        (void)fprintf(stderr, "%squal_boolop   : and\n", indent);
      }
      else if (qual->qual_boolop == 2)
      {
        (void)fprintf(stderr, "%squal_boolop   : or\n", indent);
      }
      else
      {
        (void)fprintf(stderr, "%squal_boolop   : %d ??\n", indent, 
                               qual->qual_boolop);
      }
      (void)fprintf(stderr, "%squal_nquals   : %d\n", indent, 
                             qual->qual_nquals);
      for (i=0; i<qual->qual_nquals; i++)
      {
        debug_print_qual(&qual->qual_quals[i], indent);
      }
    }
  }
}

/* ***************************************************************************
** demo_qualify_row()
**
** Check a row against the qualification descriptor. There are 4 possible
** outcomes:
**   (a) row qualifies, return 1 (1=true)
**   (b) row does not qualify, return 0 (false)
**   (c) cannot determine whether row qualifes, return -1.
**   (d) error. return -1.
**
** Note that for "c" above we will write the row for the getnext() purpose
** function. getnext() will check the "rowqualified" flag, and if it is not
** set to 1 ("true"), it will use mi_eval_am_qual() to have the server
** evaluate the row. This situation can occur if the function in the
** qualification descriptor (e.g., "equal", "lessthan", etc.) is unknown
** to the external data manager (this program).
** 
** ***************************************************************************
*/
int
demo_qualify_row (char *rowbuf, vtidemo_qual_t *qual, char *coltypes,
                  int *collength)
{
    int       rslt = 0;                  /* return value                    */
    int       intval;                    /* value, if it's an integer       */
    int       rslt_array[10];            /* for complex quals, value of subs*/
    int       i;
    char      *rowptr = rowbuf;         
    int       charlen;                   /* length of constant or col value */

    /* evaluate each node of the qualification descriptor */
    if (qual->qual_issimple)
    {
      /* skip past the "deleted/not deleted" flag */
      rowptr++;

      /* navigate to the column we're interested in */
      i = 0;
      while (i < qual->qual_column)
      {
        /* skip past the null indicator flag, then skip past the column */
        rowptr++;
        rowptr += collength[i];
        i++;
      }
      /* now skip past the null flag for the current column */
      rowptr++;

      /* *********************************************************************
      ** make the comparison. there's a fair amount of duplication in this
      ** code block, but i'm not sure it would be better with a separate
      ** table-driven function call. in any case, this way we minimize the
      ** number of "strcmp()" calls.
      **
      ** note that for the purpose of this demo we're only going to evaluate
      ** "equal", "lessthan", and "greaterthan". for any other functions, we'll
      ** pass the row back to the server and evaluate it there with the
      ** mi_eval_am_qual() function call.
      ** *********************************************************************
      */
      if (strcmp(qual->qual_funcname, "equal") == 0)
      {
        if (qual->qual_coltype == 1)
        {
          memcpy(&intval, rowptr, sizeof(int));
          if (intval == atoi(qual->qual_constant))
          {
            rslt = 1;
          }
          else
          {
            rslt = 0;
          }
        }
        else /* column is a char string */
        {
          /* which string is smaller? */
          charlen = strlen(qual->qual_constant);
          if (strlen(rowptr) < charlen)
          {
            charlen = strlen(rowptr);
          }
          if (strncmp(rowptr, qual->qual_constant, charlen) == 0)
          {
            rslt = 1;
          }
          else
          {
            rslt = 0;
          }
        }
      } /* "equal" */

      /* ****************************************************************** */
      else if (strcmp(qual->qual_funcname, "lessthan") == 0)
      {
        if (qual->qual_coltype == 1)
        {
          memcpy(&intval, rowptr, sizeof(int));
          if (intval < atoi(qual->qual_constant))
          {
            rslt = 1;
          }
          else
          {
            rslt = 0;
          }
        }
        else /* column is a char string */
        {
          /* which string is smaller? */
          charlen = strlen(qual->qual_constant);
          if (strlen(rowptr) < charlen)
          {
            charlen = strlen(rowptr);
          }
          if (strncmp(rowptr, qual->qual_constant, charlen) < 0)
          {
            rslt = 1;
          }
          else
          {
            rslt = 0;
          }
        }
      } /* "lessthan" */

      /* ****************************************************************** */
      else if (strcmp(qual->qual_funcname, "greaterthan") == 0)
      {
        if (qual->qual_coltype == 1)
        {
          memcpy(&intval, rowptr, sizeof(int));
          if (intval > atoi(qual->qual_constant))
          {
            rslt = 1;
          }
          else
          {
            rslt = 0;
          }
        }
        else /* column is a char string */
        {
          /* which string is smaller? */
          charlen = strlen(qual->qual_constant);
          if (strlen(rowptr) < charlen)
          {
            charlen = strlen(rowptr);
          }
          if (strncmp(rowptr, qual->qual_constant, rowptr, charlen) > 0)
          {
            rslt = 1;
          }
          else
          {
            rslt = 0;
          }
        }
      } /* "greaterthan" */
      else
      {

      /* ****************************************************************** */
        /* don't know how to resolve this one. let the server do it. */
        rslt = -1;
      }
      /* negate the result, if appropriate. (leave it -1 if it's unresolved) */
      if (qual->qual_negate == 1)
      {
        if (rslt == 1)
        {
          rslt = 0;
        }
        else if (rslt == 0)
        {
          rslt = 1;
        }
      }
    }
    else
    {
      /* qualification is complex */
      for (i=0; i<qual->qual_nquals; i++)
      {
        rslt_array[i] = demo_qualify_row(rowptr, &qual->qual_quals[i],
                                         coltypes, collength);
      }

      if (qual->qual_boolop == 1) /* "and" */
      {
        rslt = 1;
        for (i=0; i<qual->qual_nquals; i++)
        {
          if (rslt_array[i] == 0)
          {
            rslt = 0;
          }
          else if (rslt_array[i] == -1)
          {
            rslt = -1;
          }
        }
      }
      else /* "or" */
      {
        rslt = 0;
        for (i=0; i<qual->qual_nquals; i++)
        {
          if (rslt_array[i] == 1)
          {
            rslt = 1;
          }
          else if (rslt_array[i] == -1)
          {
            rslt = -1;
          }
        }
      } /* "or" */
    } /* complex qualification */

    return rslt;
}

