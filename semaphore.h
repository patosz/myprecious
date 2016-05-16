#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>



#define SYS_NEG(call)(((call)==-1)?(perror(#call),exit(1)):0)
#define SYS_NULL(call)(((call)==NULL)?(perror(#call),exit(1)):0)

typedef int semaphore;

semaphore mutex;
semaphore bd;

int down(int);
int up(int);


#endif