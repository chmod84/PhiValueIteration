
#include "state.h"
#include <math.h>

#define GAMMA 0.9

int run_vi(struct state* states, int state_space_size) {
    double delta = 0;
    struct state* state;
    for (state = states; state != NULL; state = (struct state*) (state->hh.next)) {
        
        double max_q = perform_bellman_update(state);
        double diff = fabs(max_q - state->v);
        if (diff > delta)
            delta = diff;
    }
    



}

double perform_bellman_update(struct state* state) {
    if (state->terminal) {
        state->v=0;
        return 0;
    }
    double max_q = -INFINITY;
    
    
    
}