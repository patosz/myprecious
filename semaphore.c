
#include "semaphore.h"



int down(int sem_id){
    struct sembuf sem_op;
    sem_op.sem_num=0; /*Num du semaphore*/
    sem_op.sem_op=-1; /*Operation sur le semaphore*/
    sem_op.sem_flg=0; /*Options pour l'operation*/

    /*semop() effectue des opérations sur les membres du jeu de sémaphores identifié par semid.*/
    SYS_NEG(semop(sem_id,&sem_op,1));
    return 0;

}


int up(int sem_id){
    struct sembuf sem_op;
    sem_op.sem_num=0; /*Num du semaphore*/
    sem_op.sem_op=+1; /*Operation sur le semaphore*/
    sem_op.sem_flg=0; /*Options pour l'operation*/

    /*semop() effectue des opérations sur les membres du jeu de sémaphores identifié par semid.*/
    SYS_NEG(semop(sem_id,&sem_op,1));
    return 0;

}