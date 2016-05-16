/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	
*/
#include "shm.h"



int mem;

key_t key;
key_t keyBD;
key_t keyM;
key_t keyC;
int shmid;
int shmidClient;
jeu* partie;
int * addr;
int * vbool;




void init_memoire(int serveur){

    /*ftok = nous permet de créer clé*/
    key = ftok("./",20000);
    keyM = ftok("./",21000);
    keyBD = ftok("./",22000);
    keyC = ftok("./",23000);

    if(serveur == 1){
        /*shmget = allouer un segment de mémoire partagée */
        SYS_NEG(shmid= shmget(key, sizeof(jeu),IPC_CREAT | 0666));

        /*semget =  créer notre sémaphore */
        SYS_NEG(mutex = semget(keyM,1,IPC_CREAT | 0660));

        SYS_NEG(bd = semget(keyBD,1,IPC_CREAT | 0660));

        /*semctl = permet d'init le semaphore à 1*/
        SYS_NEG(semctl(mutex,0,SETVAL,1));

        SYS_NEG(semctl(bd,0,SETVAL,1));

    }else{
        SYS_NEG(shmid= shmget(key, sizeof(jeu),0));

        SYS_NEG(mutex = semget(keyM,1,0));

        SYS_NEG(bd = semget(keyBD,1,0));

    }

    SYS_NEG(shmidClient = shmget(keyC, sizeof(int),IPC_CREAT | 0666));

    /*shmat = attache le segment de mémoire partagée identifié
       par shmid au  segment  de  données  du  processus  appelant*/
    addr = (int*)shmat(shmid,NULL,0);

    if(*addr < 0){
        perror("Erreur : shmat()\n");
    }

    SYS_NULL(partie = shmat(shmid,NULL,0));


}

void fermeture_memoire(){

    /*shmdt = permet à un processus de se détacher du segment.*/
    SYS_NEG(shmdt(partie));

    /*shmctl = permet d’intervenir sur ce segment (IPC_RMID = detruit)*/
    SYS_NEG(shmctl(shmid, IPC_RMID,NULL));

    SYS_NEG(shmdt(addr));

    SYS_NEG(shmctl(shmidClient, IPC_RMID,NULL));

    SYS_NEG(semctl(mutex,IPC_RMID,0));

    SYS_NEG(semctl(bd,IPC_RMID,0));

}


void ecriture_mem_partie(jeu* paramPart){
    down(bd);
    *partie = *paramPart;
    up(bd);
}

jeu * lecteur_memoire(){
    jeu * jeuARenvoyer;
        down(mutex);
        *addr = *addr +1;
        if(*addr == 1) down(bd);
        up(mutex);
        jeuARenvoyer = partie;
        down(mutex);
        *addr = *addr-1;
        if(*addr == 0)up(bd);
        up(mutex);
        return jeuARenvoyer;
    return NULL;
}