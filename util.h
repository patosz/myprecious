
/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Entête commune au client et au serveur. 
*/

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
