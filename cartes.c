#include "cartes.h"

static const char couleurs[] = {'S','C','D','H'};
static const char figures[] = {'2','3','4','5','6','7','8','9','T','V','Q','K','A'};
static const int points[] = {2,3,4,5,6,7,8,9,10,11,12,13,1};

char getCouleur(int card){
    printf("DEBUG : carte : %d\n",card);
    int idx = card % NB_COULEURS;
   // printf("DEBUG : idx : %d\n",idx);
    char coul = couleurs[idx];
    //printf("DEBUG : couleur : %c\n",coul);
    return coul;
}

int getPoints(int card){
    printf("DEBUG : carte : %d\n",card);
    int idx = card / NB_COULEURS;
   //printf("DEBUG : idx : %d\n",idx);
    int pts = points[idx];
   // printf("DEBUG : points : %d\n",pts);
    return pts;
}

char getFigure(int card){
    printf("DEBUG : carte : %d\n",card);
    int idx = card / NB_COULEURS;
   // printf("DEBUG : idx : %d\n",idx);
    char fig = figures[idx];
   // printf("DEBUG : figure : %c\n",fig);
    return fig;
}