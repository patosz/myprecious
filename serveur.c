/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	Contient la gestion du server de jeu 
*/
#include "serveur.h"


#define BUFFER_SIZE 512


static int phaseInscription = TRUE;
static joueur joueurs[MAX_JOUEUR];


int main(int argc, char** argv){
    
	int sck_srv;
    int lectureRet;
    int ecritureRet;
	int sck_cl;
    int nbrefds;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE];
	int nbJoueurs = 0;
	struct message *msg;
	struct sockaddr_in addr2;

	key_t cleShm = IDSHM;

	int retval;
	fd_set rfds;
	fd_set fdsetServeur;
	fd_set fdsetJoueurs;
	struct timeval tv;
	srand(time(NULL)); 
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = TIMEOUT_CONNECTION;
    tv.tv_usec = 0;

    //Creation d'un signal pour lors d'un ctrl c, on kill la shm
    signal(SIGINT, INThandler);

	if( (sck_srv = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
	    perror("server - Probleme socket");
	    exit(1);
    }
	

    bzero((char*)&addr,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sck_srv,(struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("server - Probleme bind");
	}

	
	if(listen(sck_srv,MAX_JOUEUR)<0){
		perror("Erreur lors de l'ecoute de demandes de connexion...\n");
	}
	fprintf(stderr,"Server en ecoute de connexions\n");

	u_int len2 = sizeof(addr2);
	FD_SET(sck_srv, &fdsetServeur);

	create_shm(IDSHM);

	if((msg = (struct message*)malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}
	
	
	//Server ready - Ecoute connection
	while(phaseInscription && nbJoueurs < MAX_JOUEUR){
		if((sck_cl = accept(sck_srv,(struct sockaddr *)&addr2,&len2))<0){
			perror("Erreur lors de l'acceptation d'un participant...\n");
            exit(1);
		}

		FD_SET(sck_cl, &fdsetJoueurs);
		fprintf(stderr,"Connexion recue de %s\n",inet_ntoa(addr2.sin_addr));

		msg = lire_msg(sck_cl,msg);
		if(msg->code == CONNEXION){
            strcpy(buffer, msg->contenu);
            fprintf(stdout, "Un joueur s'est inscrit, voici son nom : %s\n", buffer);
            msg->code= EN_ATTENTE;
            nbJoueurs++;
            ecrire_msg(sck_cl,msg);
		}
        
		if(nbJoueurs >= MAX_JOUEUR) {
			fprintf(stdout, "La limite de joueurs est atteinte! Le jeu va commencer!\n");
			break;
		}
		
        // on a inscrit le joueur précédent, on peut écouter les demandes des autres joueurs
        if((nbrefds = select(sck_srv + 1, &fdsetServeur, NULL, NULL, &tv)) < 0) {
            perror("Erreur lors de l'attente des autres joueurs...\n");
            exit(1);
        }
		
        // si le temps de connection a expire il faut arreter la boucle
        if(nbrefds == 0) {
            printf("Temps imparti de connection dépassé, debut de la partie... \n");
            break;
        }	
	}

	//Debut partie
	while(1){

	}
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