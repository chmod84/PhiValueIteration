/* 
 * echoserver.c - A simple connection-based echo server 
 * usage: echoserver <port>
 */

#include <stdio.h>
#include "state.h"


#if 0
/* 
 * Structs exported from netinet/in.h (for easy reference)
 */

/* Internet address */
struct in_addr {
    unsigned int s_addr;
};

/* Internet style socket address */
struct sockaddr_in {
    unsigned short int sin_family; /* Address family */
    unsigned short int sin_port; /* Port number */
    struct in_addr sin_addr; /* IP address */
    unsigned char sin_zero[...]; /* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
    char *h_name; /* official name of host */
    char **h_aliases; /* alias list */
    int h_addrtype; /* host address type */
    int h_length; /* length of address */
    char **h_addr_list; /* list of addresses */
}
#endif

/*
 * error - wrapper for perror
 */

struct state* states = NULL;

int main(int argc, char **argv) {
    receive_from_controller();
    printf("------ Received States: --------\n");
    print_states();
    run_vi(10, 0.001);
    printf("------ After VI: --------\n");
    print_states(states);
//    fprintf(stderr,"here2\n");
    
}
