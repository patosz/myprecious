/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Fichier d'entête pour le client
*/
#if !defined CLIENT_H
#define CLIENT_H

#include "util.h"

void check_args(int*,char**);
void get_socket(int*);
//void get_host(struct hostent* host, char* url);
//void alloc_msg(struct message* msg);
struct message* lire_msg(int sck,struct message *msg);
void ecrire_msg(int sck, struct message *msg);
#endif