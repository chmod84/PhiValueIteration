#define _GNU_SOURCE
#include <math.h>
#include "state.h"
#include "utils.h"
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#define GAMMA 0.9

struct state*** divided_states;
int* divided_states_size;
double* deltas;
pthread_barrier_t barrier_before;
pthread_barrier_t barrier_after;

unsigned int *hash_time;

int completed = 0;

void scatter_affinity(int thread_id) {
    int scatter_phy_cores = 61;
    int log_cores_per_phys_core = 4;
    int log_core_index = 0;
    int phys_core_index = 0;
    int overall_index = 0;

    //Determine whether the thread will be bound to the 1st, 2nd, 3rd or 4th
    //logical core in a given physical core
    log_core_index = floor((thread_id - 1) / scatter_phy_cores) + 1;

    //Determine which physical core the thread will be bound to
    phys_core_index = (thread_id - ((log_core_index - 1) * scatter_phy_cores));

    //Determine the specific logical core the thread will be bound to
    overall_index = log_core_index + (phys_core_index - 1) * log_cores_per_phys_core;

    //Bind the thread to its corresponding logical core
    cpu_set_t mask;
    unsigned int len = sizeof (mask);
    CPU_ZERO(&mask);
    CPU_SET(overall_index, &mask);
    sched_setaffinity(0, len, &mask);
}

double parallel_compute_q(struct state* current_state, int action_index, int my_index) {
    //    printf("inside compute_q\n");
    double q = 0;
    struct action action = current_state->actions[action_index];
    int next_state_size = action.next_states_size;
    int i = 0;
    double qs[next_state_size];
    //    #pragma simd
    struct state * next_state[next_state_size];
    for (i = 0; i < next_state_size; i++) {
        struct timeval t1, t2;
        //        gettimeofday(&t1, NULL);
        HASH_FIND_INT(states, &action.next_states[i], next_state[i]);
        //        gettimeofday(&t2, NULL);
        //        hash_time[my_index] += ((t2.tv_sec - t1.tv_sec)*1000*1000)+(t2.tv_usec - t1.tv_usec);
    }
    for (i = 0; i < next_state_size; i++) {
        qs[i] = action.probs[i]*(action.probs[i] + GAMMA * next_state[i]->v);
    }

    for (i = 0; i < next_state_size; i++) {
        q += qs[i];
    }
    return q;
}

double parallel_perform_bellman_update(struct state* current_state, int my_index) {
    //    printf("inside perform_bellman_update\n");
    if (current_state->terminal) {
        current_state->v = 0;
        return 0;
    }
    double max_q = -INFINITY;
    int action_size = current_state->action_size;
    int i;
    double q[action_size];
    for (i = 0; i < action_size; i++) {
        q[action_size] = parallel_compute_q(current_state, i, my_index);
    }
    for (i = 0; i < action_size; i++) {
        if (q[i] > max_q)
            max_q = q[i];
    }
//    current_state->v = max_q;
    return max_q;
}

void* run_vi_worker(void* arg) {
    //    printf("inside run_vi_worker\n");
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
        double max_q = parallel_perform_bellman_update(my_states[i], my_index);
        //        fprintf(stderr,"max_q=%f\n", max_q);
        my_states[i] ->v = max_q;
        double diff = fabs(max_q - v);
        if (diff > delta)
            delta = diff;
        deltas[my_index] = delta;
        i++;
    }
}

//int run_vi_parallel() {
//    //Splitting the state space according to the number of threads
//    printf("state_space_size=%d, n_thread=%d\n", state_space_size, thread_n);
//    int states_per_thread = ceil((double) state_space_size / thread_n);
//    printf("states_per_thread=%d\n", states_per_thread);
//    divided_states = malloc(sizeof (struct state**) * thread_n);
//    divided_states_size = malloc(sizeof (int)*thread_n);
//    int i, j;
//
//    for (i = 0; i < thread_n; i++) {
//        divided_states[i] = malloc(sizeof (struct state*) * states_per_thread);
//        memset(divided_states[i], 0, sizeof (struct state*) * states_per_thread);
//    }
//    struct state* current_state = states;
//    for (i = 0; i < thread_n; i++) {
//        for (j = 0; j < states_per_thread && current_state != NULL; j++) {
//            divided_states[i][j] = current_state;
//            current_state = (struct state*) (current_state->hh.next);
//        }
//        divided_states_size[i] = j;
//    }
//    //Allocating memory for deltas (1 per thread)
//    deltas = malloc(sizeof (double)*thread_n);
//    for (i = 0; i < thread_n; i++) {
//        deltas[i] = INFINITY;
//    }
//    //Creating threads
//    pthread_t threads[thread_n];
//    pthread_attr_t pthread_custom_attr;
//    pthread_attr_init(&pthread_custom_attr);
//
//
//    for (i = 0; i < iterations_n; i++) {
//        //Running threads
//        for (j = 0; j < thread_n; j++) {
//            pthread_create(&threads[j], &pthread_custom_attr, run_vi_worker, (void*) j);
//        }
//        //Joining threads
//        for (j = 0; j < thread_n; j++) {
//            pthread_join(threads[j], NULL);
//        }
//        //Computing min_delta;
//        double min_delta = INFINITY;
//        for (j = 0; j < thread_n; j++) {
//            if (deltas[j] < min_delta) {
//                min_delta = deltas[j];
//            }
//        }
//        if (min_delta < max_delta) {
//            break;
//        }
//        printf("Iteration %d, delta=%f\n", i, min_delta);
//    }
//    printf("Performed %d iterations\n", i);
//    for (i = 0; i < thread_n; i++) {
//        printf("Thread %d: %f sec in hashtable query\n", i, (hash_time[i]/(double)1000)/(double)1000);
//    }
//}

void* run_vi_worker_wrapper(void* arg) {
//    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    int thread_index = (int) arg;
    //    scatter_affinity(thread_index);
    int i, j;
    for (i = 0; i < iterations_n; i++) {
        run_vi_worker(thread_index);
        pthread_barrier_wait(&barrier_before);
        pthread_barrier_wait(&barrier_after);
        if (completed) pthread_exit(NULL);
    }

}

int run_vi_parallel_wrapped() {
    //Splitting the state space according to the number of threads
    printf("state_space_size=%d, n_thread=%d\n", state_space_size, thread_n);
    int states_per_thread = ceil((double) state_space_size / thread_n);
    printf("states_per_thread=%d\n", states_per_thread);
    divided_states = malloc(sizeof (struct state**) * thread_n);
    divided_states_size = malloc(sizeof (int)*thread_n);
    int i, j;

    for (i = 0; i < thread_n; i++) {
        divided_states[i] = malloc(sizeof (struct state*) * states_per_thread);
        memset(divided_states[i], 0, sizeof (struct state*) * states_per_thread);
    }
    struct state* current_state = states;
    int processed_states=0;
    for (i = 0; i < thread_n; i++) {
//        for (j = 0; j < states_per_thread && current_state != NULL; j++) {
        for (j = 0; j < states_per_thread && processed_states<state_space_size; j++) {
            divided_states[i][j] = current_state;
            current_state = (struct state*) (current_state->hh.next);
            processed_states++;
        }
        divided_states_size[i] = j;
    }
    //Allocating memory for deltas (1 per thread)
    deltas = malloc(sizeof (double)*thread_n);
    for (i = 0; i < thread_n; i++) {
        deltas[i] = INFINITY;
    }
    //Creating threads
    pthread_t threads[thread_n];
    pthread_attr_t pthread_custom_attr;
    pthread_attr_init(&pthread_custom_attr);


    pthread_barrier_init(&barrier_before, NULL, thread_n + 1);
    pthread_barrier_init(&barrier_after, NULL, thread_n + 1);

    hash_time = malloc(sizeof (unsigned int) * thread_n);
    for (i = 0; i < thread_n; i++) {
        hash_time[i] = 0;
    }

    for (i = 0; i < thread_n; i++) {
        pthread_create(&threads[i], &pthread_custom_attr, run_vi_worker_wrapper, (void*) i);
    }

    double min_delta = INFINITY;
    for (i = 0; i < iterations_n; i++) {
        pthread_barrier_wait(&barrier_before);
        //Computing min_delta;

        for (j = 0; j < thread_n; j++) {
            //            printf("deltas[%d]=%f\n",j,deltas[j]);
            if (deltas[j] < min_delta) {
                min_delta = deltas[j];
            }
        }
        //        printf("min_delta=%f\n",min_delta);
        if (min_delta < max_delta) {
            completed = 1;
            //            for (j = 0; j < thread_n; j++) {
            //                pthread_cancel(threads[j]);
            //            }
        }
        pthread_barrier_wait(&barrier_after);
        printf("Iteration %d, delta=%f\n", i, min_delta);
        if (completed) break;
    }
    //    printf("Iteration %d, delta=%f\n", i, min_delta);
    //    printf("Thread %d: %f sec in hashtable query\n", i, (hash_time[i]/(double)1000)/(double)1000);

    for (i = 0; i < thread_n; i++) {
        pthread_join(threads[i], NULL);
        free(divided_states[i]);
    }
    free(deltas);
    free(hash_time);
    free(divided_states_size);
    free(divided_states);
    pthread_barrier_destroy(&barrier_before);
    pthread_barrier_destroy(&barrier_after);
    completed = 0;
}