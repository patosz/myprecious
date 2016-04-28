/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15

	Contient la gestion du server de jeu 
*/
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>

#include    <sys/socket.h>
#include    <sys/types.h>

#include    <netinet/in.h>
#include	<arpa/inet.h>
#define PORT 8765
#define SYS(call) (((call)==-1)? perror(#call), exit(1) : 0)
#define BUFFER_SIZE 512
int main(int argc, char** argv){
	int sck;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE];

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

	fprintf(stderr,"Server en ecoute pour connections\n");
	listen(sck,1);

	//Server ready
	while(1){
		struct sockaddr_in addr2;
		u_int len2 = sizeof(addr2);
		int sck2 = accept(sck,(struct sockaddr *)&addr2,&len2);
		fprintf(stderr,"Connection recu de %s %d\n",inet_ntoa(addr2.sin_addr), ntohs(addr2.sin_port));
		FILE *netFd = fdopen(sck2,"r");
		while(fgets(buffer,BUFFER_SIZE,netFd)){
			fprintf(stderr, "Joueur connecte : %s \n",buffer );
		}
	}
}