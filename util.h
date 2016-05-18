
/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Entête commune au client et au serveur. 
*/

#if !defined UTIL_H

#define UTIL_H 

#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)

#define TRUE 1
#define FALSE 0

#define TIMEOUT_CONNECTION 30

#define INSCRIPTION_TIME 5

#define MIN_JOUEUR 2
#define MAX_JOUEUR 4

#define PORT 8765

#define EN_ATTENTE 0
#define CONNEXION 1
#define DECONNEXION 2
#define PARTIE_ANNULEE 3
#define DEBUT_PARTIE 4
#define FIN_PARTIE 5
#define JOUER_CARTE 6
#define ENVOI_DECK 7
#define AFFICHER_SCORE 
#define FIN_CARTES 9
#define RENVOI_CARTE 10
#define SCORE_MANCHE 11
#define FIN_MANCHE 12
#define VICTOIRE 13
#define PARTIE_EN_COURS 14
#define SERVEUR_DOWN 15

#define NB_CARTES 2

#define TAILLECONTENU 256

#define MAX_MANCHES 3

#define LOCK_FLE ".~myprecious"

typedef struct message {
	int code;
	char contenu[TAILLECONTENU];
} message;

typedef struct joueur {
	char* pseudo;
	int score;
	int carte;
} joueur;

#endif
