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
