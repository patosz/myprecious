/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Fichier d'entête pour la mémoire partagée
*/
#if !defined CLIENT_H
#define CLIENT_H

void shmem_init();
void shmem_attach();
void shmem_detach();
void shmem_get();
void shmem_grow();

#endif