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
static int tailleDeck = 0;
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
	FD_ZERO(&rfds);
    FD_SET(sck, &rfds);
    fd_set select_fds;
	while(1){
		int retSel;	
		select_fds = rfds;	
        if((retSel = select(FD_SETSIZE, &select_fds, NULL, NULL, NULL)) < 0){
        	perror("select()-client ");
        } else if(retSel == 0){
			perror("serveur down.");
			raise(SIGINT);
		}
		
        if (FD_ISSET(sck, &select_fds)){
        	recevoir_msg();
        	handleMessage(msg);
        }
	}

}

void recevoir_msg(){
	int lectureRet;
	if((lectureRet = recv(sck,msg,sizeof(struct message),0)) == -1){
		perror("erreur lecture client\n");
		raise(SIGINT);
	} else if (lectureRet == 0){
		perror("server disconnected \n");
		raise(SIGINT);
	}
}

void envoyer_msg(struct message *msg){
	int ecritureRet;
	if((ecritureRet = send(sck, msg, sizeof(struct message),0)) == -1) {
		perror("Impossible d'ecrire un message au serveur...\n");
		raise(SIGINT);
	} else if (ecritureRet == 0){
		perror("server disconnected \n");
		raise(SIGINT);
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
		case PARTIE_EN_COURS:
			partieEnCours();
			break;
		case SERVEUR_DOWN:
			serveurDown();
			break;
		default:
			printf("Message inconnu. \n");
			break;
	}
}

void serveurDown(){
	printf("Le serveur s'est coupé.. Bye !\n");
	raise(SIGINT);
}

void partieEnCours(){
	printf("Une partie est en cours... Impossible de rejoindre\n");
	raise(SIGINT);
}

void victoire(){
	printf("Vous avez gagné !\n");
	raise(SIGINT);
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
	raise(SIGINT);
}

void onDebutPartie(){
	printf("Debut de la partie.\n");
}

void onFinPartie(){
	printf("Fin de la partie.\n");
}

void onFinCartes(){
	msg->code = FIN_CARTES;
	envoyer_msg(msg);
}

void onJouerCarte(){
	printf("Voici votre deck : \n");
	int i;
	for(i = 0; i < tailleDeck; i++){
		if(deck[i] != -1){
			int card = deck[i];
			char fig = getFigure(card);
			char coul = getCouleur(card);
			printf("%d : %c%c \n",i,fig,coul);
		}
	}
	int choix = -1;
	char buff[256];
	int choixValide = FALSE;
	do{
		printf("Quelle carte voulez-vous jouer ?");
		if(fgets(buff,256*sizeof(char),stdin) != NULL && (choix = atoi(buff)) >= 0 && choix < tailleDeck && deck[choix] != -1){
			choixValide = TRUE;
		} else {
			printf("Choix invalide.\n");
		}
	}while(!choixValide);
	int card = deck[choix];
	deck[choix] = -1;
	nbCartesDeck--;
	
	printf("Carte envoyée: %d\n",card);
	msg->code = JOUER_CARTE;
	sprintf(msg->contenu,"%d",card);
	envoyer_msg(msg);
	
	
	//Si plus aucune carte
	if(nbCartesDeck == 0 && nbCartesDefausse == 0){
		msg->code = FIN_CARTES;
		printf("Vous n'avez plus de carte\n");
		envoyer_msg(msg);
		return;
	//Si encore de la carte dans la reserve
	}else if( nbCartesDeck == 0){
		memcpy(deck,defausse,nbCartesDefausse);
		nbCartesDeck = nbCartesDefausse;
		nbCartesDefausse=0;
	}
}

void onEnvoiDeck(char* contenu){
	printf("Cartes reçues. \n");
	//printf("DEBUG : Cotenu msg deck : %s\n",contenu);
	//premier strtok pour le nombre de cartes
	nbCartesDeck = tailleDeck = atoi(strtok(contenu,","));
	
	//remplir deck
	int i;
	for(i = 0; i < nbCartesDeck;i++){
		deck[i] = atoi(strtok(NULL,","));
	}
	printf("Vous avez une main de %d cartes.\n",tailleDeck);
	printf("Il vous reste %d cartes.\n",nbCartesDeck);
}

void onRenvoiCarte(char* contenu){
	int nbCartes = atoi(strtok(contenu,","));
	printf("Cartes reçues : %d\n",nbCartes);
	int i;
	for(i = 0; i < nbCartes; i++){
		int card = atoi(strtok(NULL,","));
		printf("ajout carte dans defausse : %d\n",card);
		defausse[nbCartesDefausse+i] = card;
	}
	nbCartesDefausse += nbCartes;
	printf("Vous remportez le tour.\n");
}

void onScoreManche(){
	int score = 0;
	int i;
	//points main
	for(i = 0; i < tailleDeck; i++){
		if(deck[i] != -1){
			score += getPoints(deck[i]);
		}
	}
	//points defausse
	for(i = 0; i < nbCartesDefausse; i++){
		score += getPoints(defausse[i]);
	}
	printf("Votre sore pour cette manche : %d\n",score);
	
	msg->code = SCORE_MANCHE;
	sprintf(msg->contenu,"%d",score);
	envoyer_msg(msg);
}

//Fin de la manche, on envoi le score
void onFinManche(){
	printf("Un de joueurs n'a plus de cartes. Fin de la manche.\n");
	//onScoreManche();
}

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
