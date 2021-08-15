/* ***************************************************************************
** idnSockServer.c
**
** Description: Allocates a socket and then awaits connections from a
**              client. Client sends a character string, server flips
**              it and sends it back.
**
** Parameters:  
**
**              Input: (none)
**
**              Out:   "flipped" copy of string received from client.
**
** ***************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <netdb.h>

#define BUFSIZE 100

main (argc, argv)
int	argc;
char	*argv[];
{
    struct sockaddr_in sad;  /* structure to hold server's address */
    struct sockaddr_in sad_info; /* server address info            */
    struct sockaddr_in cad;  /* structure to hold client's address */
    int    sd, sd2;          /* socket descriptors                 */
    int    port;             /* port number                        */
    int    alen;             /* address length                     */
    char   inbuf[BUFSIZE];   /* buffer for string received from client */
    char   outbuf[BUFSIZE];  /* buffer for string sent by server   */
    int    n;                /* bytes received from the client     */
    int    i, j;             /* counters                           */
    int    size;             /* for sizeof argument to getsockname */

    (void)fprintf(stderr, "\nSocket server starting...\n");

    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;             /* set family to Internet   */
    sad.sin_addr.s_addr = htons(INADDR_ANY);  /* take any available port */

    /* Create a socket */
    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0)
    {
        (void)fprintf(stderr, "socket creation failed.\n");
        exit(1);
    }

    /* Bind it */
    if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        (void)fprintf(stderr, "bind failed.\n");
        exit(1);
    }

    /* Get the port number that the system assigned us */
    size = sizeof(sad_info);
    if (getsockname(sd, (struct sockaddr *) &sad_info, &size) < 0)
    {
        (void)fprintf(stderr, "getsockname() failed.\n");
        exit(1);
    }

    port = sad_info.sin_port;

    /* Listen. We're using a request queue with a single member */
    if (listen(sd, 1) < 0)
    {
        (void)fprintf(stderr, "listen failed.\n");
        exit(1);
    }

    (void)fprintf(stderr, "Socket server listening on port %d\n", port);

    /* Main server loop: Accept and handle requests */
    while (1)
    {
        alen = sizeof(cad);
        if ((sd2 = accept(sd, (struct sockaddr *)&cad, &alen)) < 0)
        {
            (void)fprintf(stderr, "accept failed.\n");
            exit(1);
        }

        (void)fprintf(stderr, "...got a connection!\n");

        (void)fprintf(stderr, "receiving a request from a client...\n");
        n = recv(sd2, inbuf, BUFSIZE, 0);
	(void)fprintf(stderr, "got %d bytes from client...\n", n);

        /* how many bytes in the text? */
        i = 0;
        while (inbuf[i] != '\0')
        {
            i++;
        }

        /* flip the inbuf into outbuf */
        j = 0;
        outbuf[i] = '\0';
        i--;

        while (i >= 0)
        {
            outbuf[i] = inbuf[j];
            j++;
            i--;
        }

        (void)fprintf(stderr, "...sending back: %s\n\n", outbuf);

        send(sd2, outbuf, n, 0);
        close(sd2);

    } /* while */
}

