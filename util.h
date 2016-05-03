<<<<<<< HEAD

=======
<<<<<<< HEAD
#define EN_ATTENTE 0
=======
>>>>>>> be47e05d23dc5f39abd105cef5cb2e1bfd830670
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
<<<<<<< HEAD

=======
>>>>>>> 40dd142bd1dfa4236f776d451ca60d78821c5e88
>>>>>>> be47e05d23dc5f39abd105cef5cb2e1bfd830670
#define CONNEXION 1
#define DECONNECTION 2
#define PARTIE_ANNULEE 3
#define DEBUT_PARTIE 4
#define TAILLECONTENU 256

<<<<<<< HEAD
typedef struct message {
	int code;
	char contenu[TAILLECONTENU];
} message;


#endif

=======
<<<<<<< HEAD
#define TAILLECONTENU 512

typedef struct message {
	int code;
	char contenu[TAILLECONTENU];
} message;
=======
typedef struct message{
	int code_message;
	char* contenu;
} message;

#endif
>>>>>>> 40dd142bd1dfa4236f776d451ca60d78821c5e88
>>>>>>> be47e05d23dc5f39abd105cef5cb2e1bfd830670
