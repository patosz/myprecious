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

#include    <netinet/in.h>

#include    <sys/socket.h>
#include    <sys/types.h>

#define PORT 8765
#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)
#define BUFFER_SIZE 512

// Methods declaration
extern void err_handler(int);

int main(int argc, char** argv){
	int sck, lectureRet, writeRet;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr;
	struct hostent *host;
	signal(SIGPIPE,err_handler);
	struct message *msg;

	check_args(&argc, argv);
	
	get_socket(&sck);

	//TODO: cette fonction foire
	//get_host(host, argv[1]);
	host = gethostbyname(argv[1]);
	if(host == NULL){
		fprintf(stderr, "Host inconnu");
		exit(1);
	}

	alloc_msg(msg);

	prepare_sock_adr(&addr, host);
	
	/*
		//bcopy et bzero sont dépréciées selon le MAN
		memset((char*)&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_family = AF_INET;
		memcpy((char*) &addr.sin_addr.s_addr, host->h_addr, host->h_length);
		addr.sin_port = htons(PORT);	
	*/
	
	connecter_socket();
		
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

/*
 * pour modifier la valeur d'un pointeur(*), 
 * il faut déclarer un pointeur sur pointeur(**)
 * et modifier la valeur du **
 * -> C passe les parameutre en les copiant, MEME LES POINEURS !!
*/
void get_host(struct hostent** ht, char* url){
	printf("URL : %s \n",*url);
	*ht = gethostbyname(url);
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

function prepare_sock_adr(struct sockaddr_in** addr, struct hostent* host){
	//TODO finir et debugger
	memset((char*)*addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	memcpy((char*) addr->sin_addr->s_addr, host->h_addr, host->h_length);
    addr->sin_port = htons(PORT);	
}