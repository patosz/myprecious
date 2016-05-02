/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Contient la gestion joueur 
*/

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "client.h"
#include "util.h"

#define PORT 8765
#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)
<<<<<<< HEAD
#define BUFFER_SIZE 256
=======
#define BUFFER_SIZE 512

// Methods declaration
>>>>>>> 40dd142bd1dfa4236f776d451ca60d78821c5e88
extern void err_handler(int);


int main(int argc, char** argv){
<<<<<<< HEAD
	int sck,port,lectureRet,writeRet;
	char buffer[BUFFER_SIZE];
=======
	int sck;
>>>>>>> 40dd142bd1dfa4236f776d451ca60d78821c5e88
	struct sockaddr_in addr;
	struct hostent *host;
	struct message *msg;
	char buffer[BUFFER_SIZE];
	
	signal(SIGPIPE,err_handler);
	
	check_args(&argc, argv);
	fprintf(stdout, "Args ok\n");
	
	get_socket(&sck);
	fprintf(stdout, "Socket ok\n");
	
	//TODO: cette fonction foire
	//get_host(host, argv[1]);
	
	host = gethostbyname(argv[1]);
	if(host == NULL){
		fprintf(stderr, "Host inconnu");
		exit(1);
	}

	alloc_msg(msg);

	//bcopy et bzero sont dépréciées selon le MAN
	memset((char*)&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	memcpy((char*) &addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_port = htons(PORT);
	

<<<<<<< HEAD
    if(connect(sck, (struct sockaddr *)&addr,sizeof(addr)) < 0){
    	perror("Client - Probleme connect");
    	exit(1);
    }

    fprintf(stderr, "Veuillez entrer votre pseudo\n");
    
    
=======
    fprintf(stdout, "Veuillez entrer votre pseudo\n");
    /*
>>>>>>> 40dd142bd1dfa4236f776d451ca60d78821c5e88
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

void check_args(int *argc, char** argv){
	if(*argc != 2){
		fprintf(stderr, "Usage: %s  ip\n",argv[0]);
		exit(1);
	}
}

void get_socket(int *sck){
	if( (*sck = socket(AF_INET,SOCK_STREAM,0)) < 0){
		perror("Client - Probleme de socket");
		exit(1);
	}
}

void get_host(struct hostent* ht, char* url){
	printf("URL : %s \n",*url);
	ht = gethostbyname(url);
	if(ht == NULL){
		fprintf(stderr, "Host inconnu");
		exit(1);
	}
}

void alloc_msg(struct message* msg){
	if((msg = (struct message*) malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}
}