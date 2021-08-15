/* ***************************************************************************
** idnFIFOServer.c
**
** Description: Opens a FIFO ("named pipe") and reads from it. A UDR
**              client will write a character string to the pipe; the
**              string includes the name of its own FIFO, plus a string
**              of text. The server inverts the string of text and writes
**              the flipped string back to the client's FIFO.
**
** Parameters:  
**
**              Input: (none)
**
**              Out:   "flipped" copy of string received from client.
**
** ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#define SERVER_FIFO "/tmp/idn_fifo.tmp"
int
main ()
{
    char   inbuf[BUFSIZ];    /* buffer for string received from client */
    char   tmpbuf[BUFSIZ];   /* temporary buffer                   */
    char   outbuf[BUFSIZ];   /* buffer for string sent by server   */
    int    n;                /* byte counter                       */
    int    i, j;             /* counters                           */
    int    server_fd;        /* fd for the "listen" pipe           */
    int    client_secs;
    int    client_fd;        /* fd for the "send" pipe             */
    char   client_fifo[BUFSIZ]; /* name of the client fifo         */
    int    rslt;

    (void)fprintf(stderr, "\nFIFO server starting...\n");

    /* Open the server's "listen" pipe */
    rslt = mkfifo(SERVER_FIFO, (mode_t)0666);
    if (rslt < 0)
    {
        if (errno == EEXIST)
        {
          /* go ahead and delete it, then try again */
          (void)unlink(SERVER_FIFO);
          if ((rslt = mkfifo(SERVER_FIFO, (mode_t)0666)) < 0)
          {
            (void)fprintf(stderr, "\nmkfifo() failed for server FIFO!\n");
            exit(1);
          }
        }
    }

    if ((server_fd = open(SERVER_FIFO, O_RDWR, 0666)) < 0)
    {
        (void)fprintf(stderr, "\nUnable to open() server FIFO.\n");
        exit(1);
    }

    /* Read forever */
    while (1)
    {
        memset(inbuf, 0, BUFSIZ);
	(void)fprintf(stderr, "Reading...\n");
        n = read(server_fd, inbuf, (size_t)BUFSIZ);

        (void)fprintf(stderr, "Got %d bytes from client: %s\n",
                      n, inbuf);

        /* separate the seconds field from the text */
	i = 0;
	while (( i < BUFSIZ) && (inbuf[i] != ' '))
	{
	  tmpbuf[i] = inbuf[i];
	  i++;
	}
	tmpbuf[i] = '\0';
	i++;  /* skip past that space character in inbuf */

	if (i >= BUFSIZ)
	{
	  (void)fprintf(stderr, "Unable to parse input string: %s\n", inbuf);
	}
	else
	{
          if (sscanf(tmpbuf, "%d", &client_secs) != 1)
	  {
	  (void)fprintf(stderr, 
	    "Unable to extract unique client identifier: %s\n", tmpbuf);
	  }
	  else
	  {
	    /* get the text part of the buffer */
	    j = 0;

	    while (( i < BUFSIZ) && (inbuf[i] != '\0'))
	    {
	      tmpbuf[j] = inbuf[i];
	      i++;
	      j++;
	    }
	    tmpbuf[j] = '\0';

	    if (i >= BUFSIZ)
	    {
	      (void)fprintf(stderr, "Error: Buffer too big.\n");
	    }
	    else
	    {
	      /* build the client fifo name */
	      sprintf(client_fifo, "/tmp/idn_client-%d.tmp", client_secs);

	      /* open it for writing */
	      client_fd = open(client_fifo, O_RDWR, 0666);
              if (client_fd < 0)
              {
                (void)fprintf(stderr, "open() failed for client fifo.\n");
		(void)fprintf(stderr, "filename is: %s\n", client_fifo);
                exit(1);
              }

	      /* flip the text part of the string into outbuf */
	      i = strlen(tmpbuf);
              j = 0;
              outbuf[i] = '\0';
              i--;

              while (i >= 0)
              {
                outbuf[i] = tmpbuf[j];
                j++;
                i--;
              }

              /* write the flipped buffer to the client's FIFO */
              (void)fprintf(stderr, "...sending back: %s\n", outbuf);

    	      n = write(client_fd, outbuf, (size_t)strlen(outbuf) + 1);
              if (n != strlen(outbuf) + 1)
              {
                (void)fprintf(stderr, "\nUnable to write to client FIFO.\n");
                (void)fprintf(stderr, "Tried to write %d bytes, but wrote %d\n",
                              strlen(outbuf) + 1, n);
              }

              close(client_fd);
	    }
          }
       }
    } /* while */
    /* not reached */
    return 0;
}

