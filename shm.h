/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	
*/
#ifndef _SHM_H_
#define _SHM_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "util.h"
#include "semaphore.h"

typedef struct jeu{
    joueur joueurs [MAX_JOUEUR];
}jeu;

void init_memoire(int);
int down(int);
int up(int);
void fermeture_memoire();
void ecriture_mem_partie(jeu*);
jeu * lecteur_memoire();

void init_jeu(jeu * j,joueur * js,int);

#endif