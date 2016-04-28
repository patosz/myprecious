#define CONNEXION 1
#define DECONNECTION 2
#define PARTIE_ANNULEE 3
#define DEBUT_PARTIE 4

typedef struct message{
	int code_message;
	char* contenu;
} message;