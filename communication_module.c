#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

void error(char *msg) {
    perror(msg);
//    exit(1);
}

int receive_from_controller() {
    int listenfd; /* listening socket */
    int connfd; /* connection socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */

    portno = 4001;

    /* socket: create a socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets 
     * us rerun the server immediately after we kill it; 
     * otherwise we have to wait about 20 secs. 
     * Eliminates "ERROR on binding: Address already in use" error. 
     */
    optval = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
            (const void *) &optval, sizeof (int));

    /* build the server's internet address */
    bzero((char *) &serveraddr, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET; /* we are using the Internet */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* accept reqs to any IP addr */
    serveraddr.sin_port = htons((unsigned short) portno); /* port to listen on */

    /* bind: associate the listening socket with a port */
    if (bind(listenfd, (struct sockaddr *) &serveraddr,
            sizeof (serveraddr)) < 0)
        error("ERROR on binding");

    /* listen: make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 5) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");

    /* main loop: wait for a connection request, echo input line, 
       then close connection. */
    clientlen = sizeof (clientaddr);

    /* accept: wait for a connection request */
    connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
    if (connfd < 0)
        error("ERROR on accept");

    /* gethostbyaddr: determine who sent the message */
    hostp = gethostbyaddr((const char *) &clientaddr.sin_addr.s_addr,
            sizeof (clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
        error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
        error("ERROR on inet_ntoa\n");
    //        printf("server established connection with %s (%s)\n",
    //                hostp->h_name, hostaddrp);



    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    do {
        // Reading the size of the next states
        unsigned int size;
        n = read(connfd, &size, sizeof (int));
        if (n <= 0) {
            error("ERROR reading from socket 1");
            break;
        }
        //        printf("1 server received %d bytes\n", n);
        //        printf("size=%u\n", size);

        // Reading the hashcode of the current state
        int hashcode;
        n = read(connfd, &hashcode, sizeof (int));

        //        printf("2 server received %d bytes\n", n);
        //        printf("hashcode=%d\n", hashcode);

        // Reading the terminal attribute of the current state
        int terminal;
        n = read(connfd, &terminal, sizeof (int));

        //        printf("3 server received %d bytes\n", n);
        //        printf("terminal=%d\n", terminal);
        if (!terminal) {
            int i;
            for (i = 0; i < size; i++) {
                // Reading the hashcode value of the next state
                int nexthash;
                n = read(connfd, &nexthash, sizeof (int));

                //                printf("4 server received %d bytes\n", n);
                //                printf("\tnexthash=%d\n", nexthash);

                // Reading the probability value of the next state
                double prob;
                n = read(connfd, &prob, sizeof (double));

                //                printf("5 server received %d bytes\n", n);
                //                printf("\tprob=%f\n", prob);

                // Reading the reward value of the next state
                double reward;
                n = read(connfd, &reward, sizeof (double));

                //                                printf("6 server received %d bytes\n", n);
                //                printf("\treward=%f\n", reward);
            }
        }
        //        printf("\n");
    } while (n > 0);

    gettimeofday(&tv2, NULL);

    time_t seconds = tv2.tv_sec - tv1.tv_sec;
    useconds_t useconds = tv2.tv_usec - tv1.tv_usec;
    fprintf(stderr, "Transfer time: %u seconds %u milliseconds\n", seconds, useconds / 1000);

    close(connfd);
}
