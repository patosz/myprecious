 /**************************************************************
 *	DOLEGA PATRYCJUSZ pdolega15
 *	HOFFMANN XAVIER xhoffma15
 *	Projet Unix: "Bataille"
 *
 *	Fichier implémentant le comportement du client. 
 **************************************************************/
 
#include "client.h"

#define PORT 8765
#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)
#define BUFFER_SIZE 512

extern void err_handler(int);

int main(int argc, char** argv){
	int sck;
	int ecritureRet;
	int lectureRet;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr;
	struct hostent *host;
	signal(SIGPIPE,err_handler);
	struct message *msg;

	check_args(&argc, argv);
	
	get_socket(&sck);

	get_host(&host, argv[1]);
	
	alloc_msg(&msg);
	
	//bcopy et bzero sont dépréciées selon le MAN
	memset((char*)&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	memcpy((char*) &addr.sin_addr.s_addr, host->h_addr, host->h_length);
    addr.sin_port = htons(PORT);

    if(connect(sck, (struct sockaddr *)&addr,sizeof(addr)) < 0){
    	perror("Client - Probleme connect");
    	exit(1);
    }
	
	//get pseudo
    fprintf(stdout, "Veuillez entrer votre pseudo : \n");
    if((lectureRet = read(0, buffer, BUFFER_SIZE)) == -1) {
		perror("Erreur lors de la lecture au clavier...\n");
		exit(2);
	}

	//prepare connection message
	strtok(buffer, "\n");
	strcpy(msg->contenu,buffer);
	msg->code = CONNEXION;


	envoyer_msg(sck,msg);

	msg = recevoir_msg(sck,msg);
	
	handleMessage(msg->code);

	

	//ecoute message serveur
	while(1){
		
	}

}

struct message* recevoir_msg(int sck,struct message *msg){
	int lectureRet;
	if((lectureRet = read(sck,msg,sizeof(struct message))) == -1){
		perror("erreur lecture client\n");
		exit(1);
	}
	return msg;
}

void envoyer_msg(int sck, struct message *msg){
	int ecritureRet;
	if((ecritureRet = write(sck, msg, sizeof(struct message))) == -1) {
			perror("Impossible d'ecrire un message au serveur...\n");
			exit(1);
	}
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

void get_host(struct hostent** ht, char* url){
	printf("URL : %d \n",*url);
	struct hostent* h = gethostbyname(url); 
	*ht = h;
	if(ht == NULL){
		fprintf(stderr, "Host inconnu");
		exit(1);
	}
}

void alloc_msg(struct message** msg){
	if((*msg = (struct message*) malloc(sizeof(struct message))) == NULL) {
		perror("Erreur lors de l'allocation de memoire pour un message...\n");
		exit(2);
	}
}

void handleMessage(int messageCode){
	switch (messageCode){
		case EN_ATTENTE:
			printf("Attente début de la partie.\n");
			break;
		case PARTIE_ANNULEE:
			onPartieAnnulee();
			break;
		case DEBUT_PARTIE:
			onDebutPartie();	
		default:
			break;
	}
}

void onPartieAnnulee(){
	
}

void onDebutPartie(){
	
}