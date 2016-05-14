#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <errno.h>
#include <stdio.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "shm.h"
#include "util.h"

void sem_init(key_t);
void sem_up(int numSem);
void sem_down(int numSem);
int sem_destroy();
void decrement_nb_lecteurs();
void increment_nb_lecteurs();
void sem_start_read();
void sem_stop_read();


#endif