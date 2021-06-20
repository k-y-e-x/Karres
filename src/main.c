/**
 * main.c : karr�s fichier principal pour fonction main
 */
 
#include <stdio.h>

extern void init(void);
extern void choisirDificulte(void);
extern void action(void);

/**
 *	main : initialisation et lancement du puzzle
 *         �tant en mode graphique hors tgi, tout est sp�cifique pour chaque machine
 */
void main (void)
{
	init();
	choisirDificulte();
	action();
}