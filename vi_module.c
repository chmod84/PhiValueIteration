
#include <math.h>
#include "state.h"
#include <stdio.h>
#define GAMMA 0.9

double compute_q(struct state* current_state, int action_index) {
    fprintf(stderr,"3\n");
    double q = 0;
    struct action action = current_state->actions[action_index];
    int next_state_size = action.next_states_size;
    int i = 0;
    for (i = 0; i < next_state_size; i++) {
        struct state* next_state;
        fprintf(stderr, "Looking for state %d\n", action.next_states[i]);
        HASH_FIND_INT(states, &action.next_states[i], next_state);
        if (next_state==NULL)
            fprintf(stderr,"Not found!\n");
        q += action.probs[i]*(action.probs[i] + GAMMA * next_state->v);
    }
    return q;
}

double perform_bellman_update(struct state* current_state) {
    fprintf(stderr,"2\n");
    if (current_state->terminal) {
        current_state->v = 0;
        return 0;
    }
    double max_q = -INFINITY;
    int action_size = current_state->action_size;
    int i;
    for (i = 0; i < action_size; i++) {
        double q = compute_q(current_state, i);
        if (q > max_q)
            max_q = q;
    }
    current_state->v=max_q;
    return max_q;
}

int run_vi(int iterations, double max_delta) {
    fprintf(stderr,"1\n");
    double delta = 0;
    struct state* current_state;

    int i;
    for (i = 0; i < iterations; i++) {

        for (current_state = states; current_state != NULL; current_state = (struct state*) (current_state->hh.next)) {

            double max_q = perform_bellman_update(current_state);
            double diff = fabs(max_q - current_state->v);
            if (diff > delta)
                delta = diff;
        }

        if (delta < max_delta)
            break;
    }
}
