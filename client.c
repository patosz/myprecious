/* DOLEGA PATRYCJUSZ pdolega15
	HOFFMANN XAVIER xhoffma15

	Contient la gestion joueur 
*/

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
#define BUFFER_SIZE 512
extern void err_handler(int);

int main(int argc, char** argv){
	int sck,port;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr;
	struct hostent *host;
	signal(SIGPIPE,err_handler);
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

	bzero((char*)&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	bcopy(host->h_addr,(char*)&addr.sin_addr.s_addr,host->h_length);
    addr.sin_port = htons(PORT);

    fprintf(stderr, "Veuillez entrer votre pseudo\n");
    fgets(buffer,BUFFER_SIZE,stdin);

    if(connect(sck, (struct sockaddr *)&addr,sizeof(addr)) < 0){
    	perror("Client - Probleme connect");
    	exit(1);
    }
    FILE *fd = fdopen(sck,"w");
	setbuf(fd,NULL);
	fprintf(fd,"%s",buffer);

    fprintf(stderr, "Client connecte !\n");
    //Operation client
    /*
    while(1){
    	t
    }
	*/
}

void err_handler(int unused){
	fprintf(stderr, "Connection perdue ! \n");
	return;
}