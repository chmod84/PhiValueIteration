/* 
 * File:   util.h
 * Author: stefano
 *
 * Created on March 18, 2016, 11:33 AM
 */

#ifndef UTIL_H
#define	UTIL_H

#include "state.h"
void print_state(struct state*);
void print_v();
int timeval_subtract(struct timeval*, struct timeval*, struct timeval*);
extern int state_space_size;
extern int thread_n;
extern int iterations_n;
extern double max_delta;

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

