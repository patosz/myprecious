 /**************************************************************
 *	DOLEGA PATRYCJUSZ pdolega15
 *	HOFFMANN XAVIER xhoffma15
 *	Projet Unix: "Bataille"
 *
 *	Fichier implémentant le comportement du client. 
 **************************************************************/
 
#include "client.h"

#define BUFFER_SIZE 512

static int sck;
static struct sockaddr_in addr;
static struct hostent *host;
static struct message *msg;
static int nbCartesDeck = -1;
static int nbCartesDefausse = -1;
static int deck[52];
static int defausse[52];
static int partieInterrompue = FALSE;


int main(int argc, char** argv){
	int ecritureRet;
	int lectureRet;
	char buffer[BUFFER_SIZE];
	
	signal(SIGPIPE,onConnectionLost);
	signal(SIGINT,onExit);

	check_args(&argc, argv);
	
	get_socket();

	get_host(argv[1]);
	
	malloc_msg();
	
	init_address();
	
	connect_to_server();
	
	//get pseudo
    fprintf(stdout, "Veuillez entrer votre pseudo : \n");
    if((lectureRet = read(0, buffer, BUFFER_SIZE)) == -1) {
		perror("Erreur lors de la lecture au clavier...\n");
		exit(2);
	}

	//prepare connection message
	//sendConnectionMessage();
	strtok(buffer, "\n");
	strcpy(msg->contenu,buffer);
	msg->code = CONNEXION;

	envoyer_msg(msg);

	//joueur inscrit
	while(TRUE){
		//attendre le message du serveur
		msg = recevoir_msg(msg);
		//traiter le message
		handleMessage(msg);
	}
}

struct message* recevoir_msg(struct message *msg){
	int lectureRet;
	if((lectureRet = read(sck,msg,sizeof(struct message))) == -1){
		perror("erreur lecture client\n");
		exit(1);
	}
	return msg;
}

void envoyer_msg(struct message *msg){
	int ecritureRet;
	if((ecritureRet = write(sck, msg, sizeof(struct message))) == -1) {
		perror("Impossible d'ecrire un message au serveur...\n");
		exit(1);
	}
}

void check_args(int *argc, char** argv){
	if(*argc != 2){
		fprintf(stderr, "Usage: %s  ip\n",argv[0]);
		exit(1);
	}
}

void get_socket(){
	if( (sck = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("Client - Probleme de socket");
		exit(1);
	}
}

void get_host(char* url){
	printf("URL : %d \n",*url);
	host = gethostbyname(url);
	if(host == NULL){
		fprintf(stderr, "Host inconnu");
		exit(1);
	}
}

void malloc_msg(){
	if((msg = (struct message*) malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}
}

void handleMessage(struct message* msg){
	switch (msg->code){
		case EN_ATTENTE:
			printf("Attente début de la partie.\n");
			break;
		case PARTIE_ANNULEE:
			onPartieAnnulee();
			break;
		case DEBUT_PARTIE:
			onDebutPartie();
			break;
		case FIN_PARTIE:
			onFinPartie();
			break;
		case JOUER_CARTE:
			onJouerCarte();
			break;
		case ENVOI_DECK:
			onEnvoiDeck(msg->contenu);
			break;
		case RENVOI_CARTE:
			onRenvoiCarte(msg->contenu);
			break;
		case SCORE_MANCHE:
			onScoreManche();
			break;
		case FIN_MANCHE:
			onFinManche();
			break;
		default:
			printf("Message inconnu. \n");
			break;
	}
}

void init_address(){
	//bcopy et bzero sont dépréciées selon le MAN
	memset((char*)&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	memcpy((char*) &addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_port = htons(PORT);
}

void connect_to_server(){
	if(connect(sck, (struct sockaddr *)&addr,sizeof(addr)) < 0){
		perror("Client - Probleme connect");
		exit(1);
	}
}

void onPartieAnnulee(){
	printf("Partie annulée car nombre de joueurs insuffisant.\n");
	printf("Le programme va quitter.\n");
}

void onDebutPartie(){
	//attacher les mémoires
	//attendre le message avec le deck
}

void onFinPartie(){}

void onJouerCarte(){}

void onEnvoiDeck(char* contenu){}

void onRenvoiCarte(char* contenu){}

void onScoreManche(){}

void onFinManche(){}

void onConnectionLost(){
	perror("Connexion au serveur perdue ! \n");
	raise(SIGINT);
}

void onExit(){
	printf("Le programme va quitter.\n");
	//detach shared memory
	//exit
	exit(1);
}