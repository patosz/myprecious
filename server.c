/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15

	Contient la gestion du server de jeu 
*/
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
#define TIMEOUT_CONNECTION 30
int main(int argc, char** argv){
	int sck;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE];
	int nbJoueur=0;

	//Gestion du select
	int retval;
	fd_set rfds;
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
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);

	if(bind(sck,(struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("server - Probleme bind");
	}

	fprintf(stderr,"Server en ecoute pour connexions\n");
	listen(sck,1);

	//Server ready - Ecoute connection
	while(1){
		if(nbJoueur==1){
			//on lance le timer
			SYS(retval = select(1, &rfds, NULL, NULL, &tv));
		}
		//On check le timer
		if(retval ==0){
            printf("Temps imparti de connection dépassé, debut de la partie... \n");
 		}
		if(MAX_JOUEUR == nbJoueur){
			return;
		}
		struct sockaddr_in addr2;
		u_int len2 = sizeof(addr2);
		int sck2 = accept(sck,(struct sockaddr *)&addr2,&len2);
		fprintf(stderr,"Connexion recu de %s\n",inet_ntoa(addr2.sin_addr));
		FILE *netFd = fdopen(sck2,"r");
		nbJoueur++;
		while(fgets(buffer,BUFFER_SIZE,netFd)){
			fprintf(stderr, "Joueur connecte : %s \n",buffer );
		}
	}

}