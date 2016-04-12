/* 
 * echoserver.c - A simple connection-based echo server 
 * usage: echoserver <port>
 */

#include <stdio.h>
#include "state.h"
#include "utils.h"
#include <sys/time.h>
#include <unistd.h>




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
int state_space_size = 0;
int thread_n = 0;
int iterations_n = 100;
double max_delta = 0.001;

int main(int argc, char **argv) {
    receive_from_controller();
    //        print_states();
    struct timeval tv1, tv2;

    //    run_vi(100, 0.001);
    //    thread_n = atoi(argv[1]);
    //    run_vi_parallel();
    for (thread_n = 10; thread_n <= 240; thread_n = thread_n + 10) {
        struct timeval tv_average;
        int iterations;
        for (iterations = 0; iterations < 100; iterations++) {
            gettimeofday(&tv1, NULL);
            run_vi_parallel_wrapped();
            gettimeofday(&tv2, NULL);
            time_t seconds = tv2.tv_sec - tv1.tv_sec;
            useconds_t useconds = tv2.tv_usec - tv1.tv_usec;
            tv_average.tv_sec += seconds;
            tv_average.tv_usec += useconds;
            //            fprintf(stderr, "VI time: %u seconds %u microseconds\n", seconds, useconds);
            reset_v();
        }
        printf("Average planning time with %d threads: %f milliseconds\n", thread_n, ((tv_average.tv_sec * 1000000 + tv_average.tv_usec) / (double) 1000) / 1000);
    }

    //    print_v();

    //        printf("------ After VI: --------\n");
    //        print_states(states);
    //    fprintf(stderr,"here2\n");

}
