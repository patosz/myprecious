/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	Contient la gestion du server de jeu 
*/
#include "serveur.h"


#define BUFFER_SIZE 512


static int nbJoueurs = 0;
static int phaseInscription = TRUE;
static joueur joueurs[MAX_JOUEUR];
static int sockets[MAX_JOUEUR];
static struct message *msg;
static fd_set all_fds;

int main(int argc, char** argv){
    
	//TODO ajouter un fichier de lock et vérifier son exitence pour ne pas lancer 2 fois le serveur
	
	int sck_srv;
	int sck_cl;
    int nbrefds;
	int fdmax;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr2;
	key_t cleShm = IDSHM;
	fd_set read_fds;
	struct timeval tv;
	
	//Init sockets with -1 to track free places
	int i;
	for(i = 0; i < MAX_JOUEUR; i++){
		sockets[i] = -1;
	}
	
	//Creation d'un signal pour lors d'un ctrl c, on kill la shm
	struct sigaction actionInt;
	actionInt.sa_handler = INThandler;
	SYS(sigaction(SIGINT,&actionInt,0));
	
	//Creation d'un handler pour le timer
	struct sigaction actionAlarm;
	actionAlarm.sa_handler = onTimerEnd;
	SYS(sigaction(SIGALRM,&actionAlarm,0));
	
	//Init time	
	srand(time(NULL));
	
	//Empty main fd set
    FD_ZERO(&all_fds);
	
	
	//Init timeval
	tv.tv_sec = 0; //TIMEOUT_CONNECTION;
    tv.tv_usec = 500000;

    //Get socket
	if( (sck_srv = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
	    perror("server - Probleme socket");
	    exit(1);
    }

	//Init address
    bzero((char*)&addr,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Bind socket
	if(bind(sck_srv,(struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("server - Probleme bind");
	}

	//Listen on socket
	if(listen(sck_srv,MAX_JOUEUR)<0){
		perror("Erreur lors de l'ecoute de demandes de connexion...\n");
	}
	
	printf("Server en ecoute de connexions.\n");

	//Init shm
	//TODO à réparer car ne fonctionne pas
	//create_shm(IDSHM);
	
	//Malloc msg
	if((msg = (struct message*)malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}
	
	//Add server socket to set
	FD_SET(sck_srv, &all_fds);
	
	//Set server socket as highest fd
	fdmax = sck_srv;
	
	//Mask SIGALARM to not interrupt select
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGALRM);

	//Server ready - Ecoute connection
	while(phaseInscription){
		SYS(sigprocmask(SIG_BLOCK, &mask, NULL));
		
		read_fds = all_fds; // Reinit read_fds because select modifies it
        if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1) {
            perror("Erreur du select phase inscription.");
            exit(4);
        }

		// parcourir les connexions existantes en recherche d'une connexion à lire
		int i;
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
                if (i == sck_srv) {
					// handle new connections
                    u_int len2 = sizeof(addr2);
					if((sck_cl = accept(sck_srv,(struct sockaddr *)&addr2,&len2)) < 0){
						perror("Erreur lors de l'acceptation d'un participant...\n");
            			exit(1);
					} else {
						if(nbJoueurs == MAX_JOUEUR) {
							phaseInscription = FALSE;
							alarm(0);
							fprintf(stdout, "La limite de joueurs est atteinte! Le jeu va commencer!\n");
							break;
						}
						
						msg = lire_msg(sck_cl,msg);
						if(msg->code == CONNEXION){
							strcpy(buffer, msg->contenu);
							fprintf(stdout, "Un joueur s'est inscrit, voici son nom : %s\n", buffer);

							joueur j = (joueur) {.pseudo = buffer,.score = 0};
							printf("Joueur crée : %s\n",j.pseudo);
							
							if(nbJoueurs == 0){
								alarm(INSCRIPTION_TIME);
							}
														
							int idx = getFreePlace();
							joueurs[idx] = j;
							sockets[idx] = sck_cl;
							nbJoueurs++;
							
							FD_SET(sck_cl, &all_fds); // add to all_fds set
							
							if (sck_cl > fdmax) {    // keep track of the max
								fdmax = sck_cl;
							}
							printf("Connexion recue de %s\n",inet_ntoa(addr2.sin_addr));
														
							msg->code = EN_ATTENTE;
							ecrire_msg(sck_cl,msg);
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
						onPlayerLeftInscription(i);
                    } else {
                        // we got some data from a client
						msg = lire_msg(i,msg);
						
						//TODO ajouter le traitement d'un message reçu mais normalement aucun message ne devrait être reçu
                    }
                } // END handle data from client
            } // END got new incoming connection
		}
		
		//Let alarm execute it's handler
		SYS(sigprocmask(SIG_UNBLOCK, &mask, NULL));
	}
	
	printf("La phase d'inscription est terminée.\n");
	
	if(nbJoueurs >= MIN_JOUEUR){
		msg->code = DEBUT_PARTIE;
	} else {
		msg->code = PARTIE_ANNULEE;
	}
	for(i = 0; i < MAX_JOUEUR; i++){
		int socket = sockets[i]; 
		if(socket != -1){
			ecrire_msg(socket,msg);
		}
	}
	
	if(nbJoueurs < MIN_JOUEUR){
		resetPartie();
		printf("exit au lieu de reset partie \n");
		exit(1);
	}
	
	//Debut partie
	while(1){
		//ajouter logique partie
	}
}

void resetPartie(){
	//reinit sockets[]
	//reinit shmem
	//reinit nbJoueurs
	//reinit set all_fds
}

void onPlayerLeftInscription(int cl_socket){
	int i, idx;
	for(i = 0; i < MAX_JOUEUR; i++){
		if(sockets[i] == cl_socket){
			idx = i;
		}
	}
	sockets[idx] = -1;
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

struct message* lire_msg(int sck,struct message *msg){
	int lectureRet;
	if((lectureRet = read(sck,msg,sizeof(struct message))) == -1){
		perror("erreur lecture client\n");
		exit(1);
	}
	return msg;
}

void ecrire_msg(int sck, struct message *msg){
	int ecritureRet;
	if((ecritureRet = write(sck,msg,sizeof(struct message))) == -1){
		perror("Erreur ecriture server\n");
		exit(2);
	}
}

void  INThandler(int sig){
	signal(sig, SIG_IGN);
	exit(2);
}

void onTimerEnd(){
	phaseInscription = FALSE;
	printf("Le temps d'inscription est écoulé.\n");
	sigset_t mask;
	sigemptyset(&mask);
	SYS(sigprocmask(SIG_SETMASK,&mask,NULL));
}