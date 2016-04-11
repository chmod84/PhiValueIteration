/* 
 * File:   state.h
 * Author: stefano
 *
 * Created on March 17, 2016, 10:05 AM
 */

#include "uthash/uthash.h"
#include <malloc.h>

#ifndef STATE_H
#define	STATE_H

struct action {
    int hashcode;
    int next_states_size;
    int* next_states;
    double* probs;// __attribute__((aligned(64)));
    double* rewards;
};

struct state {
    int hashcode;
    int terminal;
    int action_size;
    struct action* actions;
    double v;
    UT_hash_handle hh;
};


extern struct state* states;
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* STATE_H */


