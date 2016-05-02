/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15

	Contient la gestion du server de jeu 
*/
#include "util.h"

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include	<time.h>

#include    <sys/socket.h>
#include    <sys/types.h>
#include	<sys/select.h>
#include	<sys/times.h>	

#include    <netinet/in.h>
#include	<arpa/inet.h>
#define PORT 8765
#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)
#define BUFFER_SIZE 512
#define MAX_JOUEUR 4
#define TIMEOUT_CONNECTION 10
int main(int argc, char** argv){
	int sck,lectureRet;
	int sck2,nbrefds;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE];
	int nbJoueur=0;
	struct message *msg;
	struct sockaddr_in addr2;

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


	if( (sck = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
	perror("server - Probleme socket");
	exit(1);
}


    bzero((char*)&addr,sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sck,(struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("server - Probleme bind");
	}

	fprintf(stderr,"Server en ecoute pour connexions\n");
	if(listen(sck,MAX_JOUEUR)<0){
		perror("Erreur lors de l'ecoute de demandes de connexion...\n");
	}

	u_int len2 = sizeof(addr2);
	FD_SET(sck, &fdsetServeur);


	//Server ready - Ecoute connection
	while(1){
		if((sck2 = accept(sck,(struct sockaddr *)&addr2,&len2))<0){
			perror("Erreur lors de l'acceptation d'un participant...\n");
            exit(1);
		}

		FD_SET(sck2, &fdsetJoueurs);
		fprintf(stderr,"Connexion recu de %s\n",inet_ntoa(addr2.sin_addr));

		if((msg = (struct message*)malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
		}

		if((lectureRet = read(sck2, msg, sizeof(struct message))) == -1) {
			perror("Erreur lors de la lecture du message...\n");
			exit(2);
		}
		if(msg->code == CONNEXION){
				strcpy(buffer, msg->contenu);
				fprintf(stdout, "Un joueur s'est inscrit, voici son nom : %s\n", buffer);
				msg->code= EN_ATTENTE;
				nbJoueur++;
		}
		if(nbJoueur >= MAX_JOUEUR) {
			fprintf(stdout, "La limite de joueurs est atteinte! Le jeu va commencer!\n");
			break;
		}
		// on a inscrit le joueur précédent, on peut écouter les demandes des autres joueurs
                if((nbrefds = select(sck + 1, &fdsetServeur, NULL, NULL, &tv)) < 0) {
                        perror("Erreur lors de l'attente des autres joueurs...\n");
                        exit(1);
                }
		// si le temps de connection a expire il faut arreter la boucle
                if(nbrefds == 0) {
                	printf("Temps imparti de connection dépassé, debut de la partie... \n");
                    break;
                }	
	}

}