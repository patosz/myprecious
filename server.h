/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Fichier d'entête pour le server
*/
#if !defined SERVER_H
#define SERVER_H

#include "util.h"

struct message* lire_msg(int sck,struct message *msg);
void ecrire_msg(int sck, struct message *msg);
void  INThandler(int sig);
#endif