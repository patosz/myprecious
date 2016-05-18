
/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Entête commun au client et au serveur avec les données des cartes.
*/

#if !defined CARTES_H

#define CARTES_H

#include <stdio.h>

#define NB_COULEURS 4
#define NB_FIGURES 13

int getPoints(int card);
char getCouleur(int card);
char getFigure(int card);

#endif