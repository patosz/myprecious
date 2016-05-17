#include "cartes.h"

static const char couleurs[] = {'S','C','D','H'};
static const char figures[] = {'2','3','4','5','6','7','8','9','T','V','Q','K','A'};
static const int points[] = {2,3,4,5,6,7,8,9,10,11,12,13,1};

int getPoints(int card){
    return points[card / NB_COULEURS];
}

char getCouleur(int card){
    return couleurs[card % NB_FIGURES];
}

char getFigure(int card){
    return figures[card / NB_COULEURS];
}

char* getCardText(int card){
    char txt[3];
    txt[2] = '\0';
    txt[0] = getFigure(card);
    txt[1] = getCouleur(card);
    return txt;
}