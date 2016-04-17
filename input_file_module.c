#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/time.h>
#include "state.h"
#include "utils.h"
#include <malloc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "input_file_module.h"

int read_file(char* path) {
    int n;
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        perror("ERROR on open");


    /* gethostbyaddr: determine who sent the message */
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    n = read(fd, &state_space_size, sizeof (int));
    if (n != sizeof (int)) {
        perror("ERROR in retrieving the state space size. Exiting\n");
        exit(-1);
    }
    printf("state space size: %d\n", state_space_size);
    int i;
    for (i = 0; i < state_space_size; i++) {
        if (i % 100000 == 0) {
            printf("Processed %d states\n", i);
        }
        struct state* state = malloc(sizeof (struct state));
        //Using the hashcode as base pointer for a triple read
        n = read(fd, &(state->hashcode), 3 * sizeof (int));
        if (n != 3 * sizeof (int)) {
            perror("Unable to read hashcode, terminal and actionsize attributes. Exiting\n");
            exit(-1);
        }
        int action_size = state->action_size;
        //        printf("Actions size: %d\n", action_size);
        if (action_size > 0) {
            state->actions = malloc(action_size * sizeof (struct action));
            int j;
            for (j = 0; j < action_size; j++) {
                struct action* action = state->actions + j;
                n = read(fd, &(action->next_states_size), sizeof (int));
                if (n != sizeof (int)) {
                    perror("Unable to read the size of the next states. Exiting\n");
                    exit(-1);
                }
                //                                printf("Next states size: %d\n", action->next_states_size);
                action->next_states = malloc(action->next_states_size * sizeof (int));
                action->probs = _mm_malloc(action->next_states_size * sizeof (double), 64);
                action->rewards = _mm_malloc(action->next_states_size * sizeof (double), 64);
//                action->probs = malloc(action->next_states_size * sizeof (double));
//                action->rewards = malloc(action->next_states_size * sizeof (double));
                n = read(fd, action->next_states, action->next_states_size * sizeof (int));
                n = read(fd, action->probs, action->next_states_size * sizeof (double));
                n = read(fd, action->rewards, action->next_states_size * sizeof (double));

                //                Trying to minimize the number of read
                //                int to_read = action->next_states_size * sizeof (int) +action->next_states_size * sizeof (double)*2;
                //                action->next_states = malloc(to_read);
                //                memset(action->next_states,0,to_read);
                //                action->probs = (double*) action->next_states + action->next_states_size;
                //                action->rewards = action->probs + action->next_states_size;
                //                n = 0;
                //                do {
                //                    int partial_read = read(connfd, (char*) (action->next_states + n), to_read - n);
                //                    n += partial_read;
                //                } while (n < to_read);
                //                if (n != to_read) {
                //                    fprintf(stderr, "Critical!! Expected %d, read %d\n", to_read, n);
                //                    exit(-1);
                //                }
                n = read(fd, &action->hashcode, sizeof (int));
                if (n != sizeof (int)) {
                    fprintf(stderr, "Expected %d bytes, received %d bytes\n", sizeof (int), n);
                    perror("Unable to read the action hashcode. Exiting\n");
                    exit(-1);
                }
                //                int k;
                //                for (k = 0; k < action->next_states_size; k++) {
                //                    printf("next state %d: %d\n", k, action->next_states[k]);
                //                    printf("reward: %f\n", action->rewards[k]);
                //                    printf("probability: %f\n", action->probs[k]);
                //                }
            }
        }
        state->v = (double) random();
        HASH_ADD_INT(states, hashcode, state);
//        printf("%d\n",state->hashcode);
    }
    printf("Received %d states\n", i);
    gettimeofday(&tv2, NULL);

    time_t seconds = tv2.tv_sec - tv1.tv_sec;
    useconds_t useconds = tv2.tv_usec - tv1.tv_usec;
    fprintf(stderr, "Transfer time: %u seconds %u milliseconds\n", seconds, useconds / 1000);

    close(fd);
    //    print_states(states);
}

