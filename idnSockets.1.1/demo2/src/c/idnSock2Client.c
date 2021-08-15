/* ***************************************************************************
** idnSock2Client.c
**
** Description: Initiates a socket client as a stand-alone program. Note
**              that this reverses the usual sense of client and server.
**              
**              The socket server, running in an EVP, sends a character string.
**              This program receives it, flips it, and sends it back.
**
**              On receipt of the string "Shutdown", shuts down.
**
** Parameters:  
**
**              Input: Socket server's port number.
**
**              Out:   "flipped" copy of string received from server.
**
** ***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define BUFSIZE 100

void
main (argc, argv)
int	argc;
char	*argv[];
{
    struct sockaddr_in sad;  /* structure to hold server's address */
    int    sd;               /* socket descriptor                  */
    int    port;             /* port number                        */
    struct hostent *ptrh;    /* host table entry for server        */
    char host[] = "localhost";
    char   inbuf[BUFSIZE];   /* buffer for string received from client */
    char   outbuf[BUFSIZE];  /* buffer for string sent by server   */
    int    n;                /* bytes received from the client     */
    int    i, j;             /* counters                           */
    int    rslt;
    int    one = 1;

    char  *logfile = "/tmp/idn_sockclient.log"; /* socket log path    */
    FILE   *fd;              /* file descriptor for log file       */

    char *idnGetTime();

    /* Open the log file */
    if ((fd = fopen(logfile, "ab")) == NULL)
    {
        (void)fprintf(stderr, 
          "\n%s: Client: Unable to open log file.\nExiting\n", idnGetTime());
        return;
    }

    /* quick syntax check */
    if (argc != 2)
    {
        (void)fprintf(fd, 
           "\n%s: Client: Usage: %s <port number>\n\n", idnGetTime(), argv[0]);
        return;
    }

    if ((port = atoi(argv[1])) <= 0)
    {
        (void)fprintf(fd, 
        "\n%s: Client: Invalid port specified: %s\n\n", idnGetTime(), argv[1]);
        return;
    }

    /* fork so the "system" command that invoked this can complete */
    if (fork() == 0)
    {
      (void)fprintf(fd, "%s: Client: forked.\n", idnGetTime());
      (void)fflush(fd);

    
      /* detach from any terminal */
      setsid();
      (void)fprintf(fd, "%s: Client: detached.\n", idnGetTime());
      (void)fflush(fd);

      signal(SIGINT, SIG_DFL);
      signal(SIGHUP, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      signal(SIGTERM, SIG_DFL);
    
      /* initialize the socket server address structure */
      memset((char *)&sad, 0, sizeof(sad));
      sad.sin_family = AF_INET;

      sad.sin_port = htons((u_short)port);

      /* get the ip address for the host */
      ptrh = gethostbyname(host);
      if (((char *)ptrh) == NULL)
      {
        (void)fprintf(fd, 
          "\n%s: Client: Unable to get host address.\n", idnGetTime());
        (void)fprintf(fd, 
          "\n%s: Client: Exiting with errno = %d\n", idnGetTime(), errno);
        return;
      }
 
      memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

      /* create a socket */
      sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (sd < 0)
      {
        (void)fprintf(fd, 
          "\n%s: Client: Socket creation failed.\n", idnGetTime());
        (void)fprintf(fd, 
          "\n%s: Client: Exiting with errno = %d.\n", idnGetTime(), errno);
        return;
      }

      /* connect to the socket server */
      rslt = connect(sd, (struct sockaddr *)&sad, sizeof(sad));

      if (rslt != 0)
      {
          (void)fprintf(fd, 
            "%s: Client: Unable to connect to socket server.\n", idnGetTime());
          (void)fprintf(fd, 
            "%s: Client: Exiting with errno = %d.\n", idnGetTime(), errno);
          return;
      }

      (void)fprintf(fd, 
        "%s: Client: Connected!\n", idnGetTime());

      /* Set the "keepalive" option to hold the connection */
      (void)setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (char *)&one, sizeof(one)); 

      /* Let the socket server know we're here */
      (void)sprintf(outbuf, "Client connected");
      send(sd, outbuf, strlen(outbuf), 0);

      /* Main loop: Accept and handle requests */
      while (1)
      {
          memset(inbuf, 0, (size_t)BUFSIZE);
          n = recv(sd, inbuf, BUFSIZE, 0);
          if (n > 0)
          {
            inbuf[n] = '\0';

            (void)fprintf(fd, 
             "\n%s: Client: received %d bytes: %s\n", idnGetTime(), n, inbuf);

            /* Is this the shutdown command? */
            if (strncmp(inbuf, "Shutdown", 8) == 0)
            {
              sprintf(outbuf, "Client shutting down.");
              send(sd, outbuf, strlen(outbuf), 0);
              (void)fprintf(fd, "%s: Client: Normal shut down...\n", 
                    idnGetTime());
              return;
            }

            /* flip the inbuf into outbuf */
            j = 0;
            i = strlen(inbuf);
            outbuf[i] = '\0';
            i--;

            while (i >= 0)
            {
              outbuf[i] = inbuf[j];
              j++;
              i--;
            }

            (void)fprintf(fd, "%s: Client: ...sending back: %d bytes: %s\n",
              idnGetTime(), strlen(outbuf), outbuf);
            (void)fflush(fd);

            send(sd, outbuf, strlen(outbuf), 0);
          }
          else
          {
            if (n < 0)
            {
              (void)fprintf(fd, "\n%s: Client: recv failed with %d\n", 
                    idnGetTime(), errno);
              (void)fprintf(fd, "%s: Client exiting.\n", idnGetTime());
              sprintf(outbuf, "Client recv failed. Shutting down.");
              send(sd, outbuf, strlen(outbuf), 0);
              return;
            }
            else
            {
              (void)fprintf(fd, 
              "\n%s: Client: recv returned 0 bytes. Exiting\n", idnGetTime());
              sprintf(outbuf, "Client received 0 bytes. Shutting down.");
              send(sd, outbuf, strlen(outbuf), 0);
              return;
            }
          }

      } /* while */
    }   /* fork */
}

/* ***********************************************************************
** idnGetTime()
**
** Simple utility to return a timestamp.
** ***********************************************************************
*/
char *
idnGetTime()
{
    time_t      t;
    char        *ret;
 
    t = time(NULL);
    ret = asctime(localtime(&t));
 
    /* get rid of new-line that unix adds! */
    ret[strlen(ret) - 1] = 0;
 
    return(ret);
}

