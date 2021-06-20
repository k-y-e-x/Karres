/**
 * karroric.c : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "karroric.h"


#define CHAR_HEIGHT             8

// hires = 240x200
// 240 points en groupes de 6 points => largeur de 40 groupes
#define SCREEN_WIDTH            40
#define SCREEN_HEIGHT           200

#define ROP_CPY                 0x40
#define ROP_INV                 0xc0

#define MENUX	0
#define MENUY	0
char s[38];
short score;

/**
 * bleau,vert,jaune,rouge : tableaux pour dessin des carrés unitaires
 */
char bleu[4*24];
char vert[4*24];
char jaune[4*24];
char rouge[4*24];

char posX,posY;

/**
 * carreHautBas : caractère haute ou basse pour le carré de sélection
 */
char carreHautBas[] = {
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
};

/**
 * carreGaucheDroite : caractère gauche ou droite pour le carré de sélection
 */
char carreGaucheDroite[] = {
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,	
	0x10,	
	0x10,	
	0x10,	
	0x10,	
	0x10,	
	0x10,	
};

/**
 * KarresRef : variale globale avec la forme de référence , pour test de comparaison
 */
char KarresRef[36];/* = {
	0,0,1,1,
	0,0,1,1,
	2,2,3,3,
	2,2,3,3
};*/

/**
 * carre : variable globale contenant l'état du puzzle
 */
char carre[36];/*= {
	0,0,1,1,
	0,0,1,1,
	2,2,3,3,
	2,2,3,3
};*/

///// pour l'évolution : largeur et hauteur du plateau : 4 ou 6
unsigned char largeur = 2;
unsigned char hauteur = 2;

// variable & fonctions tirées de la version Oric de cc65chess de raxiss 
// ajout de cclearxy
/*-----------------------------------------------------------------------*/
extern char hires_CharSet[96][CHAR_HEIGHT];
/*-----------------------------------------------------------------------*/
#define peek(addr)          ((unsigned char*)addr)[0]
#define poke(addr, val)     {((unsigned char*)addr)[0] = val;}
//#define deek(addr)          ((unsigned int*)addr)[0]
//#define doke(addr, val)     {((unsigned int*)addr)[0] = val;}
#define unused(x)           (void)(x)
/*-----------------------------------------------------------------------*/
static void clrmenu(void);
 static void cputs(char* s);
 static void gotoxy(int x, int y);
static void revers(char flag);
static void cclearxy(int x, int y);
// static void cprintf(char* fmt, char* s);
/*-----------------------------------------------------------------------*/
extern char hires_xpos;
extern char hires_ypos;
extern char hires_xsize;
extern char hires_ysize;
extern char hires_rop;
extern char hires_clr;
extern char* hires_src;

void hires_DrawA(void);
void hires_DrawCharA(void);
void hires_DrawClrA(void);

/*-----------------------------------------------------------------------*/
void hires_Draw(char xpos, char ypos, char xsize, char ysize, char rop, char *src)
{
  hires_xpos = xpos;
  hires_ypos = ypos;
  hires_xsize = xsize;
  hires_ysize = ysize;
  hires_rop = rop;
  hires_src = src;
  hires_DrawA();
}
/*-----------------------------------------------------------------------*/
void hires_DrawClr(char xpos, char ypos, char clr)
{
  hires_xpos = xpos;
  hires_ypos = ypos;
  hires_clr = clr;
  hires_DrawClrA();
}
/*-----------------------------------------------------------------------*/
void plat_DrawChar(char xpos, char ypos, char rop, char c)
{
  hires_xpos = xpos;
  hires_ypos = ypos;
  hires_rop = rop;
  hires_src = hires_CharSet[c-' '];
  hires_DrawCharA();
}

/*-----------------------------------------------------------------------*/
void plat_DrawString(char x, char y, char rop, char *s)
{
  while(*s)
    plat_DrawChar(x++,y, rop, *s++);
}


/**
 * afficherSelection : afficher le carré de selection de la zone à tourner
 * @param affiche : affichage ou éffacement du  carré de selection
 */
void afficherSelection(char affiche)
{
	char x,y,h;
	char inv;

	if(affiche>0)	inv = 0x80; else inv = 0x00;
	h=24+6;
	
	x= SCREEN_WIDTH/2 -largeur/2*5;
	y= SCREEN_HEIGHT/2 -hauteur/2*30;

//////// selection 2x2 => à adapter pour choix 2x2 ou 3x3
	// haut
	hires_Draw(x+posX*5,y+h*posY,6,6,inv,carreHautBas);
	hires_Draw(x+(posX+1)*5,y+h*posY,6,6,inv,carreHautBas);
	// bas
	hires_Draw(x+posX*5,y+h*(posY+2),6,6,inv,carreHautBas);
	hires_Draw(x+(posX+1)*5,y+h*(posY+2),6,6,inv,carreHautBas);

	// gauche
	hires_Draw(x+posX*5,y+h*posY+6,1,30,inv,carreGaucheDroite);
	hires_Draw(x+posX*5,y+h*(posY+1)+6,1,30,inv,carreGaucheDroite);
	// droite
	hires_Draw(x+(posX+2)*5,y+h*posY+6,1,30,inv,carreGaucheDroite);
	hires_Draw(x+(posX+2)*5,y+h*(posY+1)+6,1,30,inv,carreGaucheDroite);

}

/**
 *	afficherKarre : affiche le puzzle contenu dans la variable globale carre
 */
void afficherKarre()
{
	char i,j,x,y,h;
	
	h=24+6;
	
	x= SCREEN_WIDTH/2 -largeur/2*5;
	y= SCREEN_HEIGHT/2 -hauteur/2*30;
	
	for(i=0;i<largeur;i++)
	{
		for(j=0;j<hauteur;j++)
		{
			hires_Draw(x+i*5,y+h*j,4,6,0,carreHautBas);
			hires_Draw(x+i*5,y+h*j+6,1,24,0,carreGaucheDroite);
			hires_Draw(x+i*5,y+h*j+h,4,6,0,carreHautBas);
			hires_Draw(x+(i+1)*5,y+h*j+6,1,24,0,carreGaucheDroite);
			switch(carre[i+largeur*j])
			{
				case 0:
					hires_Draw(x+1+i*5,y+h*j+6,4,24,0,bleu);
					break;
				case 1:
					hires_Draw(x+1+i*5,y+h*j+6,4,24,0,rouge);
					break;
				case 2:
					hires_Draw(x+1+i*5,y+h*j+6,4,24,0,vert);
					break;
				case 3:
					hires_Draw(x+1+i*5,y+h*j+6,4,24,0,jaune);
					break;
			}
		}
	}

}

void initSelection()
{
	posX = largeur/2-1;
	posY = hauteur/2-1;
	afficherSelection(1);
}



void initKarre()
{
	char i,j;
	for(i=0;i<4*24;i++)
	{
		rouge[i]=bleu[i]=vert[i]=0x40;
		jaune[i]=0x40;
	}
	for(i=0;i<24;i++)
	{
 		rouge[4*i]=0x11;
 		bleu[4*i]=0x14;
		vert[4*i]=0x12;
		jaune[4*i]=0x13;
	}
	
	for(i=0;i<30;i++) carre[i]=KarresRef[i]=0;
	
	for(i=0;i<largeur;i++)
		for(j=0;j<hauteur;j++)
		{
			carre[i+j*largeur]=KarresRef[i+j*largeur]=(2*i/largeur)+(2*j/hauteur)*2;
		}
	
	afficherKarre();
	initSelection();
}

void afficherMenuDifficulte(char choix)
{
	gotoxy(0,MENUY);cputs("Difficult_ : ");
	gotoxy(15,MENUY);revers(choix==0);cputs("2x2");revers(0);
	gotoxy(20,MENUY);revers(choix==1);cputs("2x3");revers(0);
	gotoxy(25,MENUY);revers(choix==2);cputs("3x3");revers(0);
	
}

/*
 * choisierDifficulte : menu de choix d ela difficulte
 */
void choisirDificulte()
{
	char choix;
	char key;
	choix = 0;
	afficherMenuDifficulte(choix);

	do
	{
		do key = getkey(); while(!key);

		switch(key)
		{
			case KEY_LEFT:
				if(choix>0) choix--;
				break;
			case KEY_RIGHT:
				if(choix<2) choix++;
				break;
		}
		afficherMenuDifficulte(choix);
	}	while(key!=KEY_RETURN);
	cclearxy(0,MENUY);
	
	switch(choix)
	{
		case 0:
			largeur=4;
			hauteur=4;
			break;
		case 1:
			largeur=4;
			hauteur=6;
			break;
		case 2:
			largeur=6;
			hauteur=6;
			break;
	}
	
	
//	hires_DrawClr(SCREEN_WIDTH/2 -15,SCREEN_HEIGHT/2-90,15)
//	hires_DrawClr(0,SCREEN_HEIGHT/2-90,40*5);
	initKarre();
}

/**
 *	init : initialisations diverses
 */
 
void init()
{
	char key = 0;
	unsigned char i;
	char c,k;
	
	// livre ( = _ en latin1) pour é
	c=95;k=101;
	poke(46080L+c*8, 4);
	poke(46080L+c*8+1, 8);
	for(i=2;i<8;i++)
	{
		poke(46080L+c*8+i, peek(46080L+k*8+i));
	}


	setup();
	clrmenu();

	//plat_DrawString(8,20,ROP_CPY,"Karr_s Oric par kyex 2021");
	gotoxy(7,2);
	cputs("Karr_s Oric par kyex 2021");

	initKarre();	
	
	//plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,180,ROP_CPY,s);
	
}

/**
 * verifierKarres : compare le puzzle à l'état de départ
 * @return : VRAI si le puzzle est résolu
 */
char verifierKarre()
{
	char res = 1,i;
		
	//plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,"verif");
	for(i=0;i<(sizeof(carre) / sizeof(carre[0]));i++)
	{
		res = res & (KarresRef[i] == carre[i]);

	}		
	//plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,"           ");
	return(res);
}

/**
 * melangerKarres : fonction de mélange du plateau à résoudre
 * @param lvl : niveau du jeu = nombre de mouvements pour le mélange
 */
 
void melangerKarres(char lvl)
{
	int r,r1=-1,x;
	int t,t1=-1;
	char c;
	char i; 
	//char key;
	
	//sprintf(s,"melange");
	//plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
	//do key = getkey(); while(!key);

	for(i=0;i<lvl;i++)
	{
		do
		{
			r=rand() % ((largeur-1)*(hauteur-1));
			x=r%(largeur-1)+(r/(hauteur-1))*largeur;
			t=rand() & 0x01;
			c = carre[x];
// sprintf(s,"   x=%d,r=%d,t=%d,c=%d   ",x,r,t,c);
// plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
// do key = getkey(); while(!key);
			r=x;
		} while((r==r1 && t!=t1) || (carre[r+1]==c && carre[r+largeur]==c && carre[r+largeur+1]==c));
// sprintf(s," okx=%d,r=%d,t=%d,c=%d   ",x,r,t,c);
// plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
// do key = getkey(); while(!key);
		
		r1=r;
		t1=t;
		
		if(t)
		{
			c=carre[r];
			carre[r]=carre[r+largeur];
			carre[r+largeur]=carre[r+1+largeur];
			carre[r+1+largeur]=carre[r+1];
			carre[r+1]=c;
		}
		else
		{
			c=carre[r];
			carre[r]=carre[r+1];
			carre[r+1]=carre[r+1+largeur];
			carre[r+1+largeur]=carre[r+largeur];
			carre[r+largeur]=c;			
		}
			//afficherKarre();

	}
	afficherSelection(0);
	afficherKarre();
	initSelection();
	//sprintf(s,"        ");
	//plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
}

/**
 *	action : fonction principale d'affichage et gestion du puzzle
 */
void action(void)
{
	char key,res = 0, mvt=0;
	char i,c;
	int lvl;
	
	score = 0;

	lvl=1;
	
	do   // faire une limite à 10 niveauw pour le 4x4 et 15 niveaux pour le 3x3
	{
		for(i=0;i<lvl*2;i++)
		{
			score+=lvl*10;

			sprintf(s,"niveau %d (%d.%d)  ",lvl*(lvl-1)/2+1+i,lvl,i);
			plat_DrawString(0,0,ROP_CPY,s);

			melangerKarres(lvl);
			res=0;mvt=0;
			do
			{

				do key = getkey(); while(!key);

				afficherSelection(0);
				switch(key)
				{
					case KEY_LEFT:
						if(posX>0) posX--;
						break;
					case KEY_RIGHT:
						if(posX<(largeur-2)) posX++;
						break;
					case KEY_UP:
						if(posY>0) posY--;
						break;
					case KEY_DOWN:
						if(posY<(hauteur-2)) posY++;
						break;
					case KEY_RETURN:	//tourne sens horaire
						c = carre[posX+posY*largeur];
						carre[posX+posY*largeur]=carre[posX+(posY+1)*largeur];
						carre[posX+(posY+1)*largeur]=carre[posX+1+(posY+1)*largeur];
						carre[posX+1+(posY+1)*largeur]=carre[posX+1+posY*largeur];
						carre[posX+1+posY*largeur]=c;
						afficherKarre();
						mvt++;
						res = verifierKarre();
						break;
					case KEY_ESC:	//tourne sens antihoraire						
						c = carre[posX+posY*largeur];
						carre[posX+posY*largeur]=carre[posX+1+posY*largeur];
						carre[posX+1+posY*largeur]=carre[posX+1+(posY+1)*largeur];
						carre[posX+1+(posY+1)*largeur]=carre[posX+(posY+1)*largeur];
						carre[posX+(posY+1)*largeur]=c;
						afficherKarre();
						mvt++;
						res = verifierKarre();
						break;
				}		
				if(!res) afficherSelection(1);
			} while(!res);
			score-=(mvt-lvl);
			
			sprintf(s,"  R_solu en %d coups sur %d  ",mvt,lvl);
			gotoxy(20-strlen(s)/2,0);cputs(s);
			//plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2-1,180,ROP_CPY,s);
		  
			sprintf(s,"score %d",score);
			plat_DrawString(SCREEN_WIDTH-strlen(s),0,ROP_CPY,s);
		}  //for
		lvl++;
	} while(1);
}



// fonction d'affichage sur la partie texte du hires, équivalentes aux fonctions conio (source cc65chess oric par raxiss)
// seul l'éffacement des 3 lignes du bas et l'écriture est utilisé
static char* scrptr = (char*)(0xbb80+25*40);
static char inverse = 0x00;

void clrmenu(void)
{
  memset((void*)(0xbb80+25*40), 0x20, 3*40);
  scrptr = (char*)(0xbb80+25*40);
}

void cputs(char* s)
{
  while(s && *s)
    *scrptr++ = inverse | *s++;
}

void gotoxy(int x, int y)
{
  scrptr = (char*)(0xbb80+(25+y)*40+x);
}

void revers(char flag)
{
  inverse = flag? 0x80:0x00;
}

void cclearxy(int x, int y)
{
  memset((void*)(0xbb80+(25+y)*40), 0x20, 40-x);
  scrptr = (char*)(0xbb80+(25+y)*40+x);
  
}
// void cprintf(char* fmt, char* s)
// {
//   static char temp[64];
//   sprintf(temp, fmt, s);
//   cputs(temp);
// }
