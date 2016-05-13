
/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Entête commune au client et au serveur. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#if !defined UTIL_H

#define UTIL_H 

#define IDSHM 1234
#define KEY_RC 4567
#define SEM_READ 0
#define SEM_WRITE 1

#define MAX_JOUEUR 4

#define EN_ATTENTE 0
#define CONNEXION 1
#define DECONNECTION 2
#define PARTIE_ANNULEE 3
#define DEBUT_PARTIE 4
#define TAILLECONTENU 256

typedef struct message {
	int code;
	char contenu[TAILLECONTENU];
} message;

typedef struct joueur {
	char pseudo;
	int score;
}joueur;

#endif
