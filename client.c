/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15
	Contient la gestion joueur 
*/
#include "util.h"

#include    <netdb.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include 	<signal.h>
#include    <sys/socket.h>
#include    <sys/types.h>

#include    <netinet/in.h>
#define PORT 8765
#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)
#define BUFFER_SIZE 256
extern void err_handler(int);

int main(int argc, char** argv){
	int sck,port,lectureRet,writeRet;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr;
	struct hostent *host;
	signal(SIGPIPE,err_handler);
	struct message *msg;

	if(argc != 2){
		fprintf(stderr, "Usage: %s  ip\n",argv[0]);
		exit(1);
	}
	if( (sck = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("Client - Probleme de socket");
		exit(1);
	}

	host = gethostbyname(argv[1]);
	if(host == NULL){
		fprintf(stderr, "Host inconnu");
		exit(1);
	}

	if((msg = (struct message*)malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}

	bzero((char*)&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	bcopy(host->h_addr,(char*)&addr.sin_addr.s_addr,host->h_length);
    addr.sin_port = htons(PORT);

    if(connect(sck, (struct sockaddr *)&addr,sizeof(addr)) < 0){
    	perror("Client - Probleme connect");
    	exit(1);
    }

    fprintf(stderr, "Veuillez entrer votre pseudo\n");
    
    
    if((lectureRet = read(0, buffer, BUFFER_SIZE)) == -1) {
		perror("Erreur lors de la lecture au clavier...\n");
		exit(2);
	}

	strtok(buffer, "\n");

	strcpy(msg->contenu,buffer);
	msg->code = CONNEXION;
	
	if((writeRet = write(sck, msg, sizeof(struct message))) == -1) {
			perror("Impossible d'ecrire un message au serveur...\n");
			exit(1);
	}

	fprintf(stderr, "Client connecte !\n");

}

void err_handler(int unused){
	fprintf(stderr, "Connection perdue ! \n");
	return;
}