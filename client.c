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
static int nbCartesDeck = 0;
static int nbCartesDefausse = 0;
static int deck[NB_CARTES];
static int defausse[NB_CARTES];
static int partieInterrompue = FALSE;


int main(int argc, char** argv){
	int ecritureRet;
	int lectureRet;
	char buffer[BUFFER_SIZE];


	check_args(&argc, argv);
	
	//Creation d'un signal pour lors d'un ctrl c, on kill la shm
	struct sigaction actionInt;
	actionInt.sa_handler = onExit;
	SYS(sigaction(SIGINT,&actionInt,0));
	
	//Creation d'un handler pour le timer
	struct sigaction actionPipe;
	actionPipe.sa_handler = onConnectionLost;
	SYS(sigaction(SIGPIPE,&actionPipe,0));
	

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

	fd_set rfds;
	while(1){
		int retSel;
		FD_ZERO(&rfds);
        FD_SET(sck, &rfds);

        if((retSel = select(sck + 1, &rfds, NULL, NULL, NULL)) < 0){
        	perror("select()-client ");
        } else if(retSel == 0){
			perror("serveur down.");
			raise(SIGINT);
		}
		
        if (FD_ISSET(sck, &rfds)){
        	msg = recevoir_msg(msg);
        	handleMessage(msg);
        }
	}

}

struct message* recevoir_msg(struct message *msg){
	int lectureRet;
	if((lectureRet = recv(sck,msg,sizeof(struct message),0)) == -1){
		perror("erreur lecture client\n");
		exit(1);
	}
	return msg;
}

void envoyer_msg(struct message *msg){
	int ecritureRet;
	if((ecritureRet = send(sck, msg, sizeof(struct message),0)) == -1) {
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
		//OK
		case EN_ATTENTE:
			printf("Attente début de la partie.\n");
			break;
		//OK
		case PARTIE_ANNULEE:
			onPartieAnnulee();
			break;
		//OK
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
		case VICTOIRE:
			victoire();
			break;

		//Le plus simple va etre de calculer le score coté serveur et mettre a jour le score du joueur par le serveur
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
void victoire(){
	printf("Vous avez gagné !\n");
	exit(1);
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
	exit(1);
}

void onDebutPartie(){
	printf("Debut de la partie\n");
}

void onFinPartie(){
	
}

void onFinCartes(){
	msg->code = FIN_CARTES;
	envoyer_msg(msg);
}

void onJouerCarte(){
	printf("Voici votre deck : \n");
	int i;
	for(i = 0; i < NB_CARTES; i++){
		if(deck[i] != -1){
			int card = deck[i];
			printf("%d : %c%c \n",i,getFigure(card),getCouleur(card));
		}
	}
	int choix = -1;
	char buff[256];
	do{
		printf("Quelle carte voulez-vous jouer ?");
		if(fgets(buff,256*sizeof(char),stdin) != NULL && (choix = atoi(buff)) > 0 && choix < NB_CARTES && deck[choix] != -1){
			break;
		} else {
			printf("Choix invalide.\n");
		}
	}while(1);
	int card = deck[choix];
	deck[choix] = -1;
	nbCartesDeck--;
	
	msg->code = JOUER_CARTE;
	sprintf(msg->contenu,"%d",card);
	envoyer_msg(msg);

}

void onEnvoiDeck(char* contenu){
	printf("Cartes reçues. \n");
	//premier strtok pour le nombre de cartes
	nbCartesDeck = atoi(strtok(contenu,","));
	
	//remplir deck
	int i;
	for(i = 0; i < nbCartesDeck;i++){
		deck[i] = atoi(strtok(NULL,","));
	}
	printf("Vous avez une main de %d cartes.\n",nbCartesDeck);
}

void onRenvoiCarte(char* contenu){
	printf("Vous remportez le tour.\n");
}

void onScoreManche(){

}

void onFinManche(){}

void onConnectionLost(){
	perror("Connexion au serveur perdue ! \n");
	raise(SIGINT);
}

void onExit(){
	printf("Le programme va quitter.\n");
	msg->code=DECONNEXION;
	envoyer_msg(msg);
    close(sck);
	exit(1);
}

void lectureScores(){
	//On recup la memoire
	jeu *partie;
	partie = lecteur_memoire();
	printf("Voici la liste des scores actuels\n");
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		printf("Nom : %s       Score: %d\n", partie->joueurs[i].pseudo,partie->joueurs[i].score);
	}

}
