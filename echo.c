#include <stdio.h>
#include "state.h"
#include "utils.h"
#include <sys/time.h>
#include <unistd.h>
#include "input_file_module.h"


struct state* states = NULL;
int state_space_size = 0;
int thread_n = 0;
int iterations_n = 100;
double max_delta = 0.001;

int main(int argc, char **argv) {
//    receive_from_controller();
    read_file();
    //        print_states();
    struct timeval tv1, tv2, diff;

    //    run_vi(100, 0.001);
        thread_n = atoi(argv[1]);
    //    run_vi_parallel();
//    for (thread_n; thread_n < thread_n+1; thread_n = thread_n + 1) {
        long total_time = 0;
        int iterations;
        int max_iterations = 1;
        for (iterations = 0; iterations < max_iterations; iterations++) {
            gettimeofday(&tv1, NULL);
            run_vi_parallel_wrapped();
            gettimeofday(&tv2, NULL);
            timeval_subtract(&diff, &tv2, &tv1);
            long time = diff.tv_sec * 1000 + diff.tv_usec / 1000;
            printf("time: %ld\n", time);
            total_time+=time;
            printf("Total time: %ld\n", total_time);
            //            fprintf(stderr, "VI time: %u seconds %u microseconds\n", seconds, useconds);
            reset_v();
        }
        printf("Average planning time with %d threads: %ld milliseconds\n", thread_n, (total_time / max_iterations));
//    }

    //    print_v();

    //        printf("------ After VI: --------\n");
    //        print_states(states);
}
