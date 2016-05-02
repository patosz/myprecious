#define EN_ATTENTE 0
#define CONNEXION 1
#define DECONNECTION 2
#define PARTIE_ANNULEE 3
#define DEBUT_PARTIE 4

#define TAILLECONTENU 512

typedef struct message {
	int code;
	char contenu[TAILLECONTENU];
} message;