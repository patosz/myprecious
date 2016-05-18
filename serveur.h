/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Fichier d'entête pour le server
*/
#if !defined SERVER_H
#define SERVER_H
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/ipc.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>


#include "shm.h"
#include "util.h"

void recv_msg(int sck);
void send_msg(int sck, struct message *msg);
void INThandler(int sig);
void onTimerEnd();
void onPlayerLeft(int cl_socket);
int getFreePlace();
void resetPartie();
void shuffle(int *array, size_t n);
void onDebutPartie();
void sendDeck(int socket, int* deck, int nbCartes);
void checkLockFile();
int onEndPhaseInscription();
void shuffle(int *array, size_t n);
void refuserPlayer(int socket);
int getPlayerIndex(int socket);

#endif