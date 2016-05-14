/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Fichier d'entête pour le client
*/
#if !defined CLIENT_H
#define CLIENT_H


#include    <netdb.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include 	<signal.h>
#include    <unistd.h>

#include    <netinet/in.h>

#include    <sys/socket.h>
#include    <sys/types.h>

#include "util.h"

void check_args(int*,char**);
void get_socket(int*);
//void get_host(struct hostent* host, char* url);
//void alloc_msg(struct message* msg);
struct message* lire_msg(int sck,struct message *msg);
void ecrire_msg(int sck, struct message *msg);

#endif