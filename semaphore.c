
#include "semaphore.h"

static int semid;
static int* nb_lecteursSem;

void sem_init(key_t key) {
    struct sembuf sb;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if ((semid = semget(key, 2, IPC_CREAT | 0666)) < 0)
        error("Erreur à  la création du sémaphore");

    sb.sem_num = 0;
    if (semop(semid, &sb, 1) == -1)
        error("Erreur à  la création du sémaphore");

    sb.sem_num = 0;
    if (semop(semid, &sb, 1) == -1)
        error("Erreur à  la création du sémaphore");

    create_shm_nb_lecteurs(KEY_RC);
    *nb_lecteursSem =  attach_to_shm_nb_lecteurs(KEY_RC);
    *nb_lecteursSem = 0;
}

void sem_up(int n) {
    static struct sembuf sb;
    sb.sem_num = n;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

void sem_down(int n) {
    static struct sembuf sb;
    sb.sem_num = n;
    sb.sem_op -= 1;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

int sem_destroy() {
    detach_from_shm_nb_lecteurs(nb_lecteursSem);
    return semctl(semid, 0, IPC_RMID);
}

void decrement_nb_lecteurs() {
    *(nb_lecteursSem) = *(nb_lecteursSem) - 1;
    if (*nb_lecteursSem == 0) {
        sem_up(SEM_WRITE);
    }
}

void increment_nb_lecteurs() {
    *(nb_lecteursSem) = *(nb_lecteursSem) + 1;
    if (*nb_lecteursSem == 1) {
        sem_down(SEM_WRITE);
    }
}

void sem_start_read() {
    sem_down(SEM_READ);
    increment_nb_lecteurs();
    sem_up(SEM_READ);
}

void sem_stop_read() {
    sem_down(SEM_READ);
    decrement_nb_lecteurs();
    sem_up(SEM_READ);
}