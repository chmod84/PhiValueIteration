
#include <math.h>
#include "state.h"
#include "utils.h"
#include <stdio.h>
#include <pthread.h>
#define GAMMA 0.9

double compute_q(struct state* current_state, int action_index) {
    double q = 0;
    struct action action = current_state->actions[action_index];
    int next_state_size = action.next_states_size;
    int i = 0;
    for (i = 0; i < next_state_size; i++) {
        struct state* next_state;
        //        fprintf(stderr, "Looking for state %d\n", action.next_states[i]);
        HASH_FIND_INT(states, &action.next_states[i], next_state);
        if (next_state == NULL) {
            fprintf(stderr, "Critical: next state %d not found!\n", action.next_states[i]);
            exit(-1);
        }

        q += action.probs[i]*(action.rewards[i] + GAMMA * next_state->v);
        //        printf("q=%f\n",q);
    }
    return q;
}

double perform_bellman_update(struct state* current_state) {
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
    current_state->v = max_q;
    return max_q;
}

int run_vi(int iterations, double max_delta) {
    struct state* current_state;

    int i;
    for (i = 0; i < iterations; i++) {
        double delta = 0;

        //        printf("----- Iteration %d -----\n", i);
        for (current_state = states; current_state != NULL; current_state = (struct state*) (current_state->hh.next)) {
            double v = current_state->v;
            double max_q = perform_bellman_update(current_state);
            double diff = fabs(max_q - v);
            //            printf("max_q=%f, current_state->v=%f, fabs()=%f\n",max_q,current_state->v, diff);
            if (diff > delta)
                delta = diff;
            //            printf("max_q=%f\ndiff=%f\ndelta=%f\n", max_q, diff, delta);
        }

        printf("Iteration %d, delta=%f\n", i, delta);

        if (delta < max_delta)
            break;
    }
    printf("Performed %d iterations\n", i);
}