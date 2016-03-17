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
#include "state.h"

void error(char *msg) {
    perror(msg);
    //    exit(1);
}


void print_states(struct state* states) {
    struct state* state;
    for (state = states; state != NULL; state = (struct state*) (state->hh.next)) {
        printf("State %d: size of the actions: %d\n", state->hashcode, state->action_size);
        printf("\tisTerminal:%d\n", state->terminal);
        int j;
        for (j = 0; j < state->action_size; j++) {
            struct action action = state->actions[j];
            printf("\tAction %d\n", action.hashcode);
            int k;
            for (k = 0; k < action.next_states_size; k++) {
                printf("\t\tnext state hash: %d\n", action.next_states[k]);
                printf("\t\tprob: %f\n", action.probs[k]);
                printf("\t\treward: %f\n", action.rewards[k]);
            }

        }
    }
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

    int state_space_size;
    n = read(connfd, &state_space_size, sizeof (int));
    if (n <= 0) {
        error("ERROR in retrieving the state space size. Exiting\0");
        exit(-1);
    }
    printf("state space size: %d\n", state_space_size);
    int i;
    for (i = 0; i < state_space_size; i++) {
        struct state* state = malloc(sizeof (struct state));
        //Using the hashcode as base pointer for a triple read
        n = read(connfd, &(state->hashcode), 3* sizeof (int));
        int action_size = state->action_size;
//        printf("Actions size: %d\n", action_size);
        if (action_size > 0) {
            state->actions = malloc(action_size * sizeof (struct action));
            int j;
            for (j = 0; j < action_size; j++) {
                struct action action = state->actions[j];
                n = read(connfd, &(action.next_states_size), sizeof (int));
//                printf("Next states size: %d\n", action.next_states_size);
                //Trying to minimize the number of read
                action.next_states = malloc(action.next_states_size * sizeof(int) + action.next_states_size*sizeof(double)*2);
//                action.next_states = malloc(action.next_states_size * sizeof (int));
                action.probs = (double*) action.next_states+action.next_states_size;
                action.rewards = action.probs+action.next_states_size;
                n = read(connfd, action.next_states, action.next_states_size * sizeof(int) + action.next_states_size*sizeof(double)*2);
                n = read(connfd, &action.hashcode, sizeof (int));
                int k;
                for (k=0;k<action.next_states_size;k++) {
//                    printf("next state %d: %d\n", k, action.next_states[k]);
//                    printf("reward: %f\n", action.rewards[k]);
//                    printf("probability: %f\n", action.probs[k]);
                }
            }
        }
        state->v = 0;
        HASH_ADD_INT(states, hashcode, state);
    }

    gettimeofday(&tv2, NULL);

    time_t seconds = tv2.tv_sec - tv1.tv_sec;
    useconds_t useconds = tv2.tv_usec - tv1.tv_usec;
    fprintf(stderr, "Transfer time: %u seconds %u milliseconds\n", seconds, useconds / 1000);

    close(connfd);
//    print_states(states);
}

