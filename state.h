/* 
 * File:   state.h
 * Author: stefano
 *
 * Created on March 17, 2016, 10:05 AM
 */

#include "uthash/uthash.h"

#ifndef STATE_H
#define	STATE_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* STATE_H */

struct state {
    int hashcode;
    int next_states_size;
    int terminal;
    int* next_states;
    double* probs;
    double* rewards;
    double v;
    UT_hash_handle hh;
};