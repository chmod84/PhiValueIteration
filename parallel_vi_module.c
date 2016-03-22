#include <math.h>
#include "state.h"
#include "utils.h"
#include <stdio.h>
#include <pthread.h>
#define GAMMA 0.9

struct state*** divided_states;
int* divided_states_size;
double* deltas;

double parallel_compute_q(struct state* current_state, int action_index) {
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

        q += action.probs[i]*(action.probs[i] + GAMMA * next_state->v);
        //        printf("q=%f\n",q);
    }
    return q;
}

double parallel_perform_bellman_update(struct state* current_state) {
    if (current_state->terminal) {
        current_state->v = 0;
        return 0;
    }
    double max_q = -INFINITY;
    int action_size = current_state->action_size;
    int i;
    for (i = 0; i < action_size; i++) {
        double q = parallel_compute_q(current_state, i);
        if (q > max_q)
            max_q = q;
    }
    current_state->v = max_q;
    return max_q;
}

void* run_vi_worker(void* arg) {
    int my_index = (int) arg;
    //    fprintf(stderr, "Worker %d\n", my_index);
    struct state** my_states = divided_states[my_index];
    int i = 0;
    double delta = 0;
    while (i < divided_states_size[my_index]) {
//        fprintf(stderr, "i=%d\n", i);
        double v = my_states[i] ->v;
//        fprintf(stderr, "v=%f\n", v);
        //        fprintf(stderr, "Current state: %d, value: %f\n", i, v);
        double max_q = parallel_perform_bellman_update(my_states[i]);
        //        fprintf(stderr,"max_q=%f\n", max_q);
        my_states[i] ->v = max_q;
        double diff = fabs(max_q - v);
        if (diff > delta)
            delta = diff;
        deltas[my_index] = delta;
        i++;
    }
}

int run_vi_parallel(int iterations, double max_delta, int n_thread) {
    //Splitting the state space according to the number of threads
    printf("state_space_size=%d, n_thread=%d\n", state_space_size, n_thread);
    int states_per_thread = ceil((double) state_space_size / n_thread);
    printf("states_per_thread=%d", states_per_thread);
    divided_states = malloc(sizeof (struct state**) * n_thread);
    divided_states_size = malloc(sizeof (int)*n_thread);
    int i, j;

    for (i = 0; i < n_thread; i++) {
        divided_states[i] = malloc(sizeof (struct state*) * states_per_thread);
        memset(divided_states[i], 0, sizeof (struct state*) * states_per_thread);
    }
    struct state* current_state = states;
    for (i = 0; i < n_thread; i++) {
        for (j = 0; j < states_per_thread && current_state != NULL; j++) {
            divided_states[i][j] = current_state;
            current_state = (struct state*) (current_state->hh.next);
        }
        divided_states_size[i] = j;
    }
    //Allocating memory for deltas (1 per thread)
    deltas = malloc(sizeof (double)*n_thread);
    for (i = 0; i < n_thread; i++) {
        deltas[i] = INFINITY;
    }
    //Creating threads
    pthread_t threads[n_thread];
    pthread_attr_t pthread_custom_attr;
    pthread_attr_init(&pthread_custom_attr);


    for (i = 0; i < iterations; i++) {
        //Running threads
        for (j = 0; j < n_thread; j++) {
            pthread_create(&threads[j], &pthread_custom_attr, run_vi_worker, (void*) j);
        }
        //Joining threads
        for (j = 0; j < n_thread; j++) {
            pthread_join(threads[j], NULL);
        }
        //Computing min_delta;
        double min_delta = INFINITY;
        for (j = 0; j < n_thread; j++) {
            if (deltas[j] < min_delta) {
                min_delta = deltas[j];
            }
        }
        if (min_delta < max_delta) {
            break;
        }
        printf("Iteration %d, delta=%f\n", i, min_delta);
    }
    printf("Performed %d iterations\n", i);
}
