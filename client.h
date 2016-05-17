/* 
*	DOLEGA PATRYCJUSZ pdolega15
*	HOFFMANN XAVIER xhoffma15
*	Serie 2
*
*	Fichier d'entête pour le client
*/
#if !defined CLIENT_H
#define CLIENT_H


#include    <netdb.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include 	<signal.h>
#include    <unistd.h>

#include    <netinet/in.h>

#include    <sys/socket.h>
#include    <sys/types.h>

#include "util.h"
#include "shm.h"

void check_args(int* argc, char** argv);
void get_socket();
void get_host(char* url);
void malloc_msg();
void init_address();
void connect_to_server();
struct message* recevoir_msg(struct message *msg);
void envoyer_msg(struct message *msg);
void handleMessage(struct message *msg);
void onPartieAnnulee();
void onDebutPartie();
void onFinPartie();
void onJouerCarte();
void onEnvoiDeck(char* contenu);
void onRenvoiCarte(char* contenu);
void onScoreManche();
void onFinManche();
void onConnectionLost();
void onExit();
void lectureScores();
void victoire();


#endif