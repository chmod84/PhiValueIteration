#include "utils.h"
#include <stdio.h>

void print_states() {
    struct state* state;
    for (state = states; state != NULL; state = (struct state*) (state->hh.next)) {
        printf("State %d: size of the actions: %d\n", state->hashcode, state->action_size);
        printf("\tisTerminal:%d\n", state->terminal);
        printf("\tv: %f\n", state->v);
        int j;
        for (j = 0; j < state->action_size; j++) {
            struct action* action = state->actions + j;
            printf("\tAction %d\n", action->hashcode);
            int k;
            printf("\tNext state size: %d\n", action->next_states_size);
            for (k = 0; k < action->next_states_size; k++) {
                printf("\t\tnext state hash: %d\n", action->next_states[k]);
                printf("\t\tprob: %f\n", action->probs[k]);
                printf("\t\treward: %f\n", action->rewards[k]);
            }
        }
    }
}

void print_v() {
    struct state* state;
    for (state = states; state != NULL; state = (struct state*) (state->hh.next)) {
        printf("\tv: %f\n", state->v);
    }
}

void reset_v() {
    struct state* state;
    for (state = states; state != NULL; state = (struct state*) (state->hh.next)) {
        state->v = 0;
    }
}

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y) {
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}
