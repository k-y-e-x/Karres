// version mai 2021
// simplification avec appel fonctions 
// passage de constantes (define) en variables globales => remplacement dans le code
// Nouvelle version dev :
// 	- refonte menu => en haut
// 	- centrage plateau de jeu donc redécoupage des zones l'écran
// 	- passage de define en variables pour nouveau menu d'options/variantes
//  - écran titre
#include <stdio.h>
//#include "karres.h"


//extern UWORD gPosEval;

//UBYTE jeu[LGPLAT], prof[2], tab[20], j[2];

extern void init(void);
extern void test(void);

/**
 *	Initialisation des variables globales  du jeu (tableaux)
 */
// void initPartie(BOOLEAN tout)
// {
// 	//UBYTE i;
// 
// }

/**
 *	Main : boucle principale des parties et boucle de chaque partie pour humain & ordi
 */
void main (void)
{
	//UBYTE i;
	//UBYTE c;
	//BOOLEAN b=TRUE, rejoue = TRUE;


	init();

	//initPlateau();
	//ecranTitre();

	test();

}