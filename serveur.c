/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	Contient la gestion du server de jeu 
*/
#include "serveur.h"


#define BUFFER_SIZE 512


static int nbJoueurs = 0;
static int phaseInscription = TRUE;
static joueur *joueurs;
static int sockets[MAX_JOUEUR];
static struct message *msg;
static fd_set all_fds;
static int sck_srv;
static int mancheEnCours = TRUE;
static int nbManchesJouees = 0;
static jeu *je;
static int maxManches = MAX_MANCHES;

int main(int argc, char** argv){
	
	//Check if nor already running !!
	checkLockFile();
	
	if(argc == 2){
		int manchesDemandees = atoi(argv[1]);
		if(manchesDemandees <= 0){
			printf("Manches demandées négative ou nulles. La valeur par défaut est prise.\n");
		} else if(manchesDemandees > maxManches){
			printf("Manches demandées supérieures à la valeur maximale autorisée. La valeur par défaut est prise.\n");
		} else {
			maxManches = manchesDemandees;
		}
	}
	
	int sck_cl;
	struct sockaddr_in addr_srv;
	struct sockaddr_in addr2;
	fd_set read_fds;
	
	
	//Malloc joueur joueurs[MAX_JOUEUR]
	joueurs =(joueur*) malloc (MAX_JOUEUR* sizeof (joueur));

	//Malloc msg
	if((msg = (struct message*)malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}
	
	//malloc de jeu + creation de la memoire
	if ((je = malloc(sizeof(jeu))) == NULL) {
		perror("malloc () jeu : serveur");
	}
	init_memoire(1);
	ecriture_mem_partie(je);
	
	//Init sockets with -1 to track free places
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		sockets[i] = -1;
	}
	
	//Creation d'un signal pour lors d'un ctrl c, on kill la shm
	struct sigaction actionInt;
	actionInt.sa_handler = INThandler;
	SYS(sigaction(SIGINT,&actionInt,NULL));
	
	//Creation d'un handler pour le timer
	struct sigaction actionAlarm;
	actionAlarm.sa_handler = onTimerEnd;
	SYS(sigaction(SIGALRM,&actionAlarm,NULL));
	
	//Init time	
	srand(time(NULL));
	
    //Get socket
	if( (sck_srv = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
	    perror("server - Probleme socket");
	    exit(1);
    }

	//Init address
	memset((char*)&addr_srv, 0, sizeof(struct sockaddr_in));
	addr_srv.sin_family = AF_INET;
	addr_srv.sin_port = htons(PORT);
	addr_srv.sin_addr.s_addr = htonl(INADDR_ANY);

	//Bind socket
	if(bind(sck_srv,(struct sockaddr *)&addr_srv, sizeof(addr_srv)) < 0){
		perror("server - Probleme bind");
		exit(1);
	}

	//Listen on socket
	if(listen(sck_srv,MAX_JOUEUR)<0){
		perror("Erreur lors de l'ecoute de demandes de connexion...\n");
		exit(1);
	}
	
	printf("Server en ecoute de connexions.\n");

	//Empty main fd set
    FD_ZERO(&all_fds);
	//Add server socket to set
	FD_SET(sck_srv, &all_fds);
		
	//Boucle principale serveur
	while(1){
		onPhaseInscription();
		
		int annulee = onEndPhaseInscription();
		
		if(annulee == -1){
			resetPartie();
			continue;
		} else {
			distribuerCartes();
		}
		
		jouerJeu();
	}
}

void jouerJeu(){
	//Deroulement de la partie
	int nbJoueursRestants = nbJoueurs;
	int nbJoueurAyantJouer = 0;
	while(nbManchesJouees < maxManches && nbJoueurs > 1){
		jouerManche();
	}
	//TODO déterminer gagnant
	//TODO envoyer victoire au gagnant
	//TODO envoyer fin de partie à ttlm
	//TODO reset partie
}

void jouerManche(){
	while(mancheEnCours){
		jouerTour();
	}
	nbManchesJouees++;
	//demander les scores pour la manche
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		if(sockets[i] != -1){
			msg->code = FIN_MANCHE;
			send_msg(sockets[i],msg);
		}
	}
	updateScoresShmem();
}

void jouerTour(){
	//demander cartes
	int i;
	printf("jenvoi une demande\n");
	for(i = 0; i < MAX_JOUEUR; i++){
		if(sockets[i] != -1){
			msg->code = JOUER_CARTE;
			send_msg(sockets[i],msg);
		}
	}

	int cartes[MAX_JOUEUR];
	for(i=0;i<MAX_JOUEUR;i++){
		cartes[i]=-1;
	}

	int attenteCartes = TRUE;
	int sck_cl;
	struct sockaddr_in addr2;
	fd_set 	read_fds;
	int nbJoueurAyantJouer = 0;
	while(attenteCartes){
		read_fds = all_fds;
		if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Erreur du select tour.");
			raise(SIGINT);
		}
		for(i = 0; i < FD_SETSIZE; i++){
			if(FD_ISSET(i,&read_fds)){
				printf("socket : %d\n",i);
				if(i == sck_srv){
					u_int len2 = sizeof(addr2);
					if((sck_cl = accept(sck_srv,(struct sockaddr *)&addr2,&len2)) < 0){
						perror("Erreur lors de l'acceptation d'un participant...\n");
						exit(1);
					}
					printf("nouveau joueur mais partie en cours\n");
					refuserPlayer(i);
				} else {
					int retVal;
					
					if ((retVal = recv(i, msg, sizeof(struct message), 0)) <= 0) {
						// got error or connection closed by client
						if (retVal == 0) {
							// connection closed
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						onPlayerLeft(i);
					} else {
						if(msg->code == JOUER_CARTE){
							int idxP = getPlayerIndex(i);
							if(cartes[idxP] == -1){
								printf("un joueur a jouer la carte: %d\n", atoi(msg->contenu));
								nbJoueurAyantJouer++;
								cartes[idxP] = atoi(msg->contenu);
							}
							if(nbJoueurAyantJouer == nbJoueurs){
								printf("tout les joueurs ont joué\n");
								attenteCartes = FALSE;
							}
						} else if(msg->code == FIN_CARTES){
							attenteCartes = FALSE;
							mancheEnCours = FALSE;
						}
					}
				}
			}
		}
	}
	
	//définir gagnant tour
	int max=-1;
	int idxGagnant=-1;
	for(i=0;i<MAX_JOUEUR;i++){
		if(cartes[i] > max){
			idxGagnant = i;
			max = cartes[i];
		}
	}

	//renvoyer cartes au gagnant
	char buffCartes[TAILLECONTENU];
	sprintf(buffCartes,"%d",nbJoueurs);
	for(i = 0; i < MAX_JOUEUR; i++){
		if(cartes[i] != -1){
			sprintf(buffCartes,"%s,%d",buffCartes,cartes[i]);
		}
	}
	
	//CECI NE FONCTIONNE PAS IdxGagnant est à -1. Je trouve vraiment pas comment faire
	msg->code=RENVOI_CARTE;
	sprintf(msg->contenu,"%s",buffCartes);
	printf("index du gagnant %d\n", idxGagnant);
	send_msg(sockets[idxGagnant],msg);
	printf("Le gagant a recu son deck\n");

}

void updateScoresShmem(){
	int attenteScores = TRUE;
	fd_set 	read_fds;
	int sck_cl;
	int i;
	int nbJoueurAyantJouer = 0;
	struct sockaddr_in addr2;
	
	while(attenteScores){
		read_fds = all_fds;
		if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Erreur du select tour.");
			raise(SIGINT);
		}
		
		for(i = 0; i < FD_SETSIZE; i++){
			if(FD_ISSET(i,&read_fds)){
				printf("socket : %d\n",i);
				if(i == sck_srv){
					u_int len2 = sizeof(addr2);
					if((sck_cl = accept(sck_srv,(struct sockaddr *)&addr2,&len2)) < 0){
						perror("Erreur lors de l'acceptation d'un participant...\n");
						exit(1);
					}
					printf("nouveau joueur mais partie en cours\n");
					refuserPlayer(i);
				} else {
					int retVal;
					if ((retVal = recv(i, msg, sizeof(struct message), 0)) <= 0) {
						// got error or connection closed by client
						if (retVal == 0) {
							// connection closed
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						onPlayerLeft(i);
					} else {
						recv_msg(i);
						if(msg->code == SCORE_MANCHE){
							int idxP = getPlayerIndex(i);
							printf("un joueur a envoyé son score: %d\n", atoi(msg->contenu));
							nbJoueurAyantJouer++;
							printf("ici\n");
							je =  lecteur_memoire();
							printf("ou ici\n");
							je->joueurs[idxP].score += atoi(msg->contenu);
							printf("coucou\n");
							ecriture_mem_partie(je);									
							if(nbJoueurAyantJouer == nbJoueurs){
								printf("tout les joueurs ont donné leurs scores\n");
								attenteScores = FALSE;
							}
						}
					}
				}
			}
		}
	}
}

void onPhaseInscription(){
	//Mask SIGALARM to not interrupt select
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGALRM);
	
	//Init timeval
	static struct timeval tv;
	tv.tv_sec = 0;
    tv.tv_usec = 500000;
	
	fd_set read_fds;
	struct sockaddr_in addr2;
	char buffer[BUFFER_SIZE];
	int sck_cl;
	while(phaseInscription){
		SYS(sigprocmask(SIG_BLOCK, &mask, NULL));
		
		read_fds = all_fds;
        if (select(FD_SETSIZE+1, &read_fds, NULL, NULL, &tv) == -1) {
            perror("Erreur du select phase inscription.");
            exit(4);
        }

		// parcourir les connexions existantes en recherche d'une connexion à lire
		int i;
		for(i = 0; i <= FD_SETSIZE; i++) {
			if (FD_ISSET(i, &read_fds)) {
				// handle new connections
                if (i == sck_srv) {
                    u_int len2 = sizeof(addr2);
					if((sck_cl = accept(sck_srv,(struct sockaddr *)&addr2,&len2)) < 0){
						perror("Erreur lors de l'acceptation d'un participant...\n");
            			raise(SIGINT);
					} else {
						if(nbJoueurs == MAX_JOUEUR) {
							phaseInscription = FALSE;
							alarm(0);
							fprintf(stdout, "La limite de joueurs est atteinte! Le jeu va commencer!\n");
							break;
						}
						
						recv_msg(sck_cl);
						if(msg->code == CONNEXION){
							strcpy(buffer, msg->contenu);
							fprintf(stdout, "Un joueur s'est inscrit, voici son nom : %s\n", buffer);

							
							//joueur j = (joueur) {.pseudo = buffer,.score = 0};
							
							
							if(nbJoueurs == 0){
								alarm(INSCRIPTION_TIME);
							}
														
							int idx = getFreePlace();
							joueurs[idx] = (joueur) {.pseudo = buffer,.score = 0};
							sockets[idx] = sck_cl;
							printf("Joueur crée : %s et son socket est %d \n",joueurs[idx].pseudo,sockets[idx]);
							nbJoueurs++;
								
							
							FD_SET(sck_cl, &all_fds); // add to all_fds set
						
							printf("Connexion recue de %s\n",inet_ntoa(addr2.sin_addr));
														
							msg->code = EN_ATTENTE;
							send_msg(sck_cl,msg);
						}
                    }
                } else {
                    // handle data from an existing client
					int retVal;
                    if ((retVal = recv(i, msg, sizeof(struct message), 0)) <= 0) {
                        // got error or connection closed by client
                        if (retVal == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
						onPlayerLeft(i);
                    } else {
                        // we got some data from a client
						recv_msg(i);
						
						//TODO ajouter le traitement d'un message reçu mais normalement aucun message ne devrait être reçu
                    }
                } // END handle data from client
            } // END got new incoming connection
		}
		
		//Let alarm execute it's handler
		SYS(sigprocmask(SIG_UNBLOCK, &mask, NULL));
	}
}

int getPlayerIndex(int socket){
	int i;
	for(i = 0; i < nbJoueurs; i++){
		if(sockets[i] == socket){
			return i;
		}
	}
	return -1;
}

void refuserPlayer(int socket){
	msg->code = PARTIE_EN_COURS;
	send_msg(socket,msg);
	printf("%d\n", msg->code);
}

void checkLockFile(){
	FILE *fp = fopen (LOCK_FLE, "r");
	if (fp != NULL){
		fclose (fp);
		perror("Le serveur est déjà lancé. \n");
		exit(15);	
	} else {
		fp = fopen(LOCK_FLE,"w");
		fclose(fp);
	}
}

void distribuerCartes(){
	//Debut partie
	
	//Creation du deck de carte
	int deck[NB_CARTES];
	
	int t;
	for(t=0;t<NB_CARTES;t++){
		deck[t]=t;
	}
	
	shuffle(deck,NB_CARTES);
	
	//Composer et envoyer les decks
	if(nbJoueurs == 3){
		int cardsLeft = NB_CARTES;
		int nbCardsPlayer = cardsLeft / nbJoueurs;
		int i, j, h = 0;
		int lastPlayer = 0;
		for(i = 0; i < nbJoueurs; i++){
			if(i == nbJoueurs-1){
				cardsLeft = nbCardsPlayer;
			}
			
			int deckP[nbCardsPlayer];
			for(j = 0;j<nbCardsPlayer;j++, h++){
				deckP[j] = deck[h];
			}
			cardsLeft -= nbCardsPlayer;
			for(;lastPlayer < MAX_JOUEUR;lastPlayer++){
				if(sockets[lastPlayer] != -1){
					sendDeck(sockets[lastPlayer],deckP, nbCardsPlayer);
					break;
				}
			}
		}
	} else {
		int nbCardsPlayer = NB_CARTES / nbJoueurs;
		int i, j, h = 0;
		int lastPlayer = 0;
		for(i = 0; i < nbJoueurs; i++){
			int deckP[nbCardsPlayer];
			for(j = 0;j<nbCardsPlayer;j++, h++){
				deckP[j] = deck[h];
			}
			for(;lastPlayer < MAX_JOUEUR;lastPlayer++){
				if(sockets[lastPlayer] != -1){
					sendDeck(sockets[lastPlayer],deckP, nbCardsPlayer);
					lastPlayer++;
					break;
				}
			}
		}
	}
	printf("fin methode distribuerCartes\n");
}

void sendDeck(int socket, int* deck, int nbCartes){
	char buffer[TAILLECONTENU];
	int i;
	sprintf(buffer,"%d",nbCartes);
	for(i = 0; i < nbCartes; i++){
		sprintf(buffer,"%s,%d",buffer,*(deck+i));
	}
	msg->code = ENVOI_DECK;
	sprintf(msg->contenu,"%s",buffer);
	send_msg(socket,msg);
	printf("Cartes envoyées sur socket %d.\n",socket);
}

int onEndPhaseInscription(){
	printf("La phase d'inscription est terminée.\n");

	//Check si debut partie ou annulation partie et envoyer msg adequat
	msg->code = (nbJoueurs >= MIN_JOUEUR) ? DEBUT_PARTIE : PARTIE_ANNULEE;  

	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		int socket = sockets[i];
		if(socket != -1){
			send_msg(socket,msg);
		}
	}
	
	//If partie annulee, reset la partie à 0
	if(nbJoueurs < MIN_JOUEUR){
		printf("La partie est annulée et sera réinitialisée.\n");
		return -1;
	} else {
		return 0;
	}
}

void resetPartie(){
	
	printf("réinitialisation des variables \n");	
	phaseInscription = TRUE;
	phaseInscription = TRUE;
	mancheEnCours = TRUE;
	nbJoueurs = 0;
	nbManchesJouees = 0;

	printf("réinitialisation et fermeture des sockets \n");
	//reinit sockets[]
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		int sck = sockets[i];
		if(sck != -1){
			close(sck);
		}
		sockets[i] = -1;
	}
	
	printf("réinitialisation de la mémoire partagée \n");
	//reinit shmem
	for(i = 0; i < MAX_JOUEUR; i++){
		sprintf(je->joueurs[i].pseudo,"");
		je->joueurs[i].score = 0;
	}
	ecriture_mem_partie(je);
	
	printf("réinitialisation du set des fd \n");
	//reinit set all_fds
	FD_ZERO(&all_fds);
	FD_SET(sck_srv,&all_fds);
	
	printf("Partie réinitialisée.\n");
}

void onPlayerLeft(int cl_socket){
	int i, idx;
	for(i = 0; i < MAX_JOUEUR; i++){
		if(sockets[i] == cl_socket){
			sockets[i] = -1;
			break;
		}
	}
	nbJoueurs --;	
	close(cl_socket); // bye!
	FD_CLR(cl_socket, &all_fds); // remove from all_fds set
}

int getFreePlace(){
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		if(sockets[i] == -1){
			return i;
		}
	}
	return -1;
}

void recv_msg(int sck){
	int lectureRet;
	if((lectureRet = recv(sck,msg,sizeof(struct message),0)) == -1){
		perror("erreur lecture client\n");
		onPlayerLeft(sck);
	} else if(lectureRet == 0){
		perror("client closed");
		onPlayerLeft(sck);
	}
}

void send_msg(int sck, struct message *msg){
	int ecritureRet;
	if((ecritureRet = send(sck,msg,sizeof(struct message),0)) == -1){
		perror("Erreur ecriture server\n");
		onPlayerLeft(sck);
	} else if(ecritureRet == 0){
		perror("client disconnected \n");
		onPlayerLeft(sck);
	}
}

void  INThandler(int sig){
	printf("fermeture des sockets.\n");
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		int sck = sockets[i];
		if(sck != -1){
			close(sck);
		}
	}
	printf("sockets fermés.\n");
	printf("fermeture memoire partagee.\n");
	fermeture_memoire();
	printf("mémoire partagée fermée.\n");
	printf("suppression fichier lock.\n");
	remove(LOCK_FLE);
	printf("fichier lock supprimé");
	printf("fermeture du programme.\n");
	exit(2);
}

void onTimerEnd(){
	phaseInscription = FALSE;
	printf("Le temps d'inscription est écoulé.\n");
	sigset_t mask;
	sigemptyset(&mask);
	SYS(sigprocmask(SIG_SETMASK,&mask,NULL));
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n){
    if (n > 1){
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}
