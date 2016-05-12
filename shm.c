/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	
*/
#include "shm.h"


/* Id de la shm */
int shmid;
/*nb de lecteurs de la shm */
int nb_lecteurs;


/* Creer la mémoire partagé et mets son id dans shmid */
void create_shm(key_t key){
    if (shmid = shmget(key, sizeof(joueur[MAX_JOUEUR]),0644 | IPC_CREAT)<0){
        perror("Erreur lors de la création de la mémoire partagée");
    }
}
/* The only argument, shmaddr, is the address you got from shmat(). The function returns -1 on error, 0 on success
   Detache la shm apd de l'adresse
 */
void detach_shm(void* shmaddr){
    if(shmdt(shmaddr) < 0){
        perror("Erreur au détachement de la memoire partagé");
    }
}
/* Supprime completement la shm */
void delete_shm(){
    if (shmctl(shmid,IPC_RMID,NULL)<0){
        perror("Erreur lors de la destruction de la mémoire partagé");
    }
}

int *attacher_shm(key_t key,int is_writer){
    static long shmaddr;
    if ((shmid = shmget(key, sizeof(joueur[MAX_JOUEUR]), 0)) < 0){
        perror("Erreur lors de l'obtention de la memoire partagé");
    }
    if ((shmaddr = (long) shmat(shmid, 0, (is_writer ? 0 : SHM_RDONLY))) < 0){
        perror("Erreur lors de l'attachement de la mémoire partagé");
    }
    return (int *) shmaddr;
}

/*  Shm qui s'occupe du nb de lecteur pour l'algo de Courtois */

void create_shm_nb_lecteurs(key_t key) {
    if ((nb_lecteurs = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0)
        error("Erreur a la creation de la memoire partagee pour l'algo de Courtois");
}

int* attach_to_shm_nb_lecteurs(key_t key) {
    static long addresse_shm;

    if ((nb_lecteurs = shmget(key, sizeof(int), 0)) < 0)
        error("Impossible de recup la memoire partagee, elle est surement pas creer");

    if ((addresse_shm = (long) shmat(nb_lecteurs, 0, 0)) < 0)
        error("Erreur lors de l attachement de la mem partager pour l'algo de Courtois");

    return (int *) addresse_shm;
}


void detach_from_shm_nb_lecteurs(void * shm) {
    if (shmctl(nb_lecteurs, IPC_RMID, NULL) < 0)
        error("Erreur a la suppresion de la mem partagee (Courtois)");
    if (shmdt(shm) < 0)
        error("Erreur lors de detachement de la meme partagee (Courtois)");
}


