
/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Entête commune au client et au serveur. 
*/

#if !defined UTIL_H
#define EN_ATTENTE 0
#define UTIL_H 

#define CONNEXION 1
#define DECONNECTION 2
#define PARTIE_ANNULEE 3
#define DEBUT_PARTIE 4
#define TAILLECONTENU 256

typedef struct message {
	int code;
	char contenu[TAILLECONTENU];
} message;


#endif
