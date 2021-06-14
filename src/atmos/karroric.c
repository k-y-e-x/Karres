// version mai 2021
// simplification avec appel fonctions 
// passage de constantes (define) en variables globales => remplacement dans le code
// Nouvelle version dev :
// 	- refonte menu => en haut
// 	- centrage plateau de jeu donc redécoupage des zones l'écran
// 	- passage de define en variables pour nouveau menu d'options/variantes
//  - écran titre
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "karroric.h"
#include "plat.h"

#define BOARD_X                 1
#define BOARD_Y                 18

#define BOARD_PIECE_WIDTH       3
#define BOARD_PIECE_HEIGHT      22

#define CHAR_HEIGHT             8
#define LOG_WINDOW_HEIGHT       23

#define SCREEN_WIDTH            40
#define SCREEN_HEIGHT           200

#define ROP_CPY                 0x40
#define ROP_INV                 0xc0


char s[38];

char bleu[4*24];
char vert[4*24];
char jaune[4*24];
char rouge[4*24];

char posX,posY;

char carreHautBas[] = {
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
	0x10,0x40,0x40,0x40,0x40,0x40,
};

	
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

char KarresRef[] = {
	0,0,1,1,
	0,0,1,1,
	2,2,3,3,
	2,2,3,3
};
char carre[] = {
	0,0,1,1,
	0,0,1,1,
	2,2,3,3,
	2,2,3,3
};
/*-----------------------------------------------------------------------*/
static char moveBuf[7];
/*-----------------------------------------------------------------------*/
extern char hires_CharSet[96][CHAR_HEIGHT];
extern char hires_Pieces[4*7*BOARD_PIECE_WIDTH*BOARD_PIECE_HEIGHT];

/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
#define peek(addr)          ((unsigned char*)addr)[0]
#define poke(addr, val)     {((unsigned char*)addr)[0] = val;}
#define deek(addr)          ((unsigned int*)addr)[0]
#define doke(addr, val)     {((unsigned int*)addr)[0] = val;}
#define unused(x)           (void)(x)
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
static void clrmenu(void);
static void cputs(char* s);
static void gotoxy(int x, int y);
static void revers(char flag);
static void cprintf(char* fmt, char* s);


/*-----------------------------------------------------------------------*/
extern char hires_xpos;
extern char hires_ypos;
extern char hires_xsize;
extern char hires_ysize;
extern char hires_rop;
extern char hires_clr;
extern char* hires_src;
void hires_MaskA(void);
void hires_DrawA(void);
void hires_DrawCharA(void);
void hires_DrawClrA(void);
void hires_SelectA(void);

/*-----------------------------------------------------------------------*/
void hires_Mask(char xpos, char ypos, char xsize, char ysize, char rop)
{
  hires_xpos = xpos;
  hires_ypos = ypos;
  hires_xsize = xsize;
  hires_ysize = ysize;
  hires_rop = rop;
  hires_MaskA();
}

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
void hires_Select(char xpos, char ypos, char xsize, char ysize, char rop, char *src)
{
  hires_xpos = xpos;
  hires_ypos = ypos;
  hires_xsize = xsize;
  hires_ysize = ysize;
  hires_rop = rop;
  hires_src = src;
  hires_SelectA();
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

/*-----------------------------------------------------------------------*/
void plat_UpdateScreen(void)
{
  memset(moveBuf,0x20,6); moveBuf[6] = 0;
  plat_DrawString(SCREEN_WIDTH-6,(LOG_WINDOW_HEIGHT+1)*CHAR_HEIGHT,
                  ROP_CPY,moveBuf);

  /*
  switch(gOutcome)
  {
    case OUTCOME_INVALID:
    case OUTCOME_OK:
    case OUTCOME_CHECK:
    case OUTCOME_CHECKMATE:
    case OUTCOME_DRAW:
    case OUTCOME_STALEMATE:
    case OUTCOME_MENU:
    case OUTCOME_ABANDON:
    case OUTCOME_QUIT:
      break;
  }
  */
}

/*-----------------------------------------------------------------------*/
void plat_DrawBoard(char clearLog)
{
  int i;

  if(clearLog)
    hires_Mask(27,0,SCREEN_WIDTH-27,SCREEN_HEIGHT,ROP_CPY);

  // Redraw all tiles
  for(i=0; i<64; ++i)
    plat_DrawSquare(i);
}

/*-----------------------------------------------------------------------*/
void plat_DrawSquare(char position)
{
  char y = position / 8, x = position & 7;
  char piece = ' ';//gChessBoard[y][x];
  char blackWhite = !((x & 1) ^ (y & 1));
  char pieceBlackWhite = 0;//!(piece & PIECE_WHITE);

  unsigned int idx = blackWhite*7*BOARD_PIECE_HEIGHT ;//+ (piece&PIECE_DATA);

  if(piece) idx += pieceBlackWhite*2*7*BOARD_PIECE_HEIGHT;

  hires_Draw(BOARD_X+x*BOARD_PIECE_WIDTH,BOARD_Y+y*BOARD_PIECE_HEIGHT,
             BOARD_PIECE_WIDTH,BOARD_PIECE_HEIGHT,ROP_CPY,
             &hires_Pieces[idx*BOARD_PIECE_WIDTH]);

  // Show the attack numbers
//   if(gShowAttackBoard)
//   {
//     char val[4];
//     char rop = blackWhite ? ROP_INV : ROP_CPY;
// 
//     sprintf(val,"%02X%d",gChessBoard[y][x],(gChessBoard[y][x]&PIECE_WHITE)>>7);
// 
//     plat_DrawChar(BOARD_X+x*BOARD_PIECE_WIDTH,BOARD_Y+(y+1)*BOARD_PIECE_HEIGHT-CHAR_HEIGHT,
//                   rop,(gpAttackBoard[giAttackBoardOffset[position][0]])+'0');
//     plat_DrawChar(BOARD_X+(x+1)*BOARD_PIECE_WIDTH-1,BOARD_Y+(y+1)*BOARD_PIECE_HEIGHT-CHAR_HEIGHT,
//                   rop,(gpAttackBoard[giAttackBoardOffset[position][1]])+'0');
//     plat_DrawString(BOARD_X+x*BOARD_PIECE_WIDTH,BOARD_Y+y*BOARD_PIECE_HEIGHT,rop,val);
//   }
}


//extern UWORD gPosEval;


 
 
void afficheSelection(char affiche)
{
	char x,y,h;
	char inv;
	
	if(affiche>0)	inv = 0x80; else inv = 0x00;
	h=24+6;
	
	x= SCREEN_WIDTH/2 -2*5;
	y= SCREEN_HEIGHT/2 - 2*30;	

	hires_Draw(x+posX*5,y+h*posY,6,6,inv,carreHautBas);
	hires_Draw(x+(posX+1)*5,y+h*posY,6,6,inv,carreHautBas);
	hires_Draw(x+posX*5,y+h*(posY+2),6,6,inv,carreHautBas);
	hires_Draw(x+(posX+1)*5,y+h*(posY+2),6,6,inv,carreHautBas);


	hires_Draw(x+posX*5,y+h*posY+6,1,30,inv,carreGaucheDroite);
	hires_Draw(x+posX*5,y+h*(posY+1)+6,1,30,inv,carreGaucheDroite);
	hires_Draw(x+(posX+2)*5,y+h*posY+6,1,30,inv,carreGaucheDroite);
	hires_Draw(x+(posX+2)*5,y+h*(posY+1)+6,1,30,inv,carreGaucheDroite);

}

void afficherKarre()
{
	char i,j,x,y,h;
	
	h=24+6;
	
	x= SCREEN_WIDTH/2 -2*5;
	y= SCREEN_HEIGHT/2 - 2*30;
	
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			hires_Draw(x+i*5,y+h*j,4,6,0,carreHautBas);
			hires_Draw(x+i*5,y+h*j+6,1,24,0,carreGaucheDroite);
			hires_Draw(x+i*5,y+h*j+h,4,6,0,carreHautBas);
			hires_Draw(x+(i+1)*5,y+h*j+6,1,24,0,carreGaucheDroite);
			switch(carre[i+4*j])
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


/**
 *	init : initialisations diverses
 */
 
void init()
{
  char key = 0;
	unsigned char i,j,h;
	char c,k;
	//unsigned char x,y;
	
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

  // Show credits and wait for key press
//   plat_DrawString(2,SCREEN_HEIGHT/2-10-CHAR_HEIGHT/2,
//                   ROP_CPY,gszAbout);
  plat_DrawString(8,20,ROP_CPY,"Karr_s Oric par kyex 2021");
//   plat_DrawString(0,0,
//                   ROP_CPY,"0123456789012345");

  //n = 0*2*7*BOARD_PIECE_HEIGHT + 0*7*BOARD_PIECE_HEIGHT + (KING-1);
//  hires_Draw(SCREEN_WIDTH/2-2,SCREEN_HEIGHT/2-50-BOARD_PIECE_HEIGHT/2,


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
	
	afficherKarre();
		

	posX = 1;
	posY = 1;
	
	
	//afficheSelection(1);
	
	
	sprintf(s,"carr_ : %d", (sizeof(carre) / sizeof(carre[0])));
	plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,180,ROP_CPY,s);
	
}

char verifierKarre()
{
	char res = 1,i;
	char key;
	plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,"verif");

	for(i=0;i<(sizeof(carre) / sizeof(carre[0]));i++)
	{
		res = res & (KarresRef[i] == carre[i]);
//		sprintf(s,"i=%d res = %d:%d,%d",i,res,KarresRef[i] ,carre[i]);
//		plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
//		do key = getkey(); while(!key);

	}		
	plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,"           ");
	return(res);

}

void melangerKarres(char lvl)
{
	int r,r1=-1,x;
	int t,t1=-1;
	char c;
	char i; 
	char key;
	
		sprintf(s,"melange");
		plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
		//do key = getkey(); while(!key);

	for(i=0;i<lvl;i++)
	{
		do
		{
			r=rand() % 9;
			x=r%3+(r/3)*4;
			t=rand() & 0x01;
			c = carre[x];
// sprintf(s,"   x=%d,r=%d,t=%d,c=%d   ",x,r,t,c);
// plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
// do key = getkey(); while(!key);
r=x;
		} while((r==r1 && t!=t1) || (carre[r+1]==c && carre[r+4]==c && carre[r+5]==c));
// sprintf(s," okx=%d,r=%d,t=%d,c=%d   ",x,r,t,c);
// plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
// do key = getkey(); while(!key);
		
		r1=r;
		t1=t;
		
		if(t)
		{
			c=carre[r];
			carre[r]=carre[r+4];
			carre[r+4]=carre[r+5];
			carre[r+5]=carre[r+1];
			carre[r+1]=c;
		}
		else
		{
			c=carre[r];
			carre[r]=carre[r+1];
			carre[r+1]=carre[r+5];
			carre[r+5]=carre[r+4];
			carre[r+4]=c;			
		}
			//afficherKarre();

	}
	afficherKarre();
	afficheSelection(1);
	sprintf(s,"        ");
	plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,172,ROP_CPY,s);
}

/**
 *	test : test utilisation hires
 */
void test (void)
{
	char key,res = 0, mvt=0;
	char i,j,c;
	//plat_DrawBoard(1);
	int lvl;
	int score = 10;

	lvl=1;
	
	do
	{
		for(i=0;i<lvl;i++)
		{
		score+=lvl;
		//sprintf(s,"score %d",score);
		//plat_DrawString(SCREEN_WIDTH-strlen(s),6,ROP_CPY,s);

		sprintf(s,"niveau %d (%d.%d)",lvl*(lvl-1)/2+1+i,lvl,i);
		plat_DrawString(0,6,ROP_CPY,s);

		melangerKarres(lvl);
		res=0;mvt=0;
		do
		{

			do key = getkey(); while(!key);

			afficheSelection(0);
			switch(key)
			{
				case KEY_LEFT:
					if(posX>0) posX--;
					break;
				case KEY_RIGHT:
					if(posX<2) posX++;
					break;
				case KEY_UP:
					if(posY>0) posY--;
					break;
				case KEY_DOWN:
					if(posY<2) posY++;
					break;
				case KEY_RETURN:	//tourne sens horaire
					c = carre[posX+posY*4];
					carre[posX+posY*4]=carre[posX+(posY+1)*4];
					carre[posX+(posY+1)*4]=carre[posX+1+(posY+1)*4];
					carre[posX+1+(posY+1)*4]=carre[posX+1+posY*4];
					carre[posX+1+posY*4]=c;
					afficherKarre();
					mvt++;
					res = verifierKarre();
					break;
				case KEY_ESC:	//tourne sens antihoraire
					c = carre[posX+posY*4];
					carre[posX+posY*4]=carre[posX+1+posY*4];
					carre[posX+1+posY*4]=carre[posX+1+(posY+1)*4];
					carre[posX+1+(posY+1)*4]=carre[posX+(posY+1)*4];
					carre[posX+(posY+1)*4]=c;
					afficherKarre();
					mvt++;
					res = verifierKarre();
					break;
			}		
			afficheSelection(1);
		} while(!res);
		score-=(mvt-lvl);
		sprintf(s,"R_solu en %d coups sur %d",mvt,lvl);
		  plat_DrawString(SCREEN_WIDTH/2-strlen(s)/2,180,ROP_CPY,s);
		  
		  sprintf(s,"score %d",score);
		  plat_DrawString(SCREEN_WIDTH-strlen(s),6,ROP_CPY,s);

		}  //for
		lvl++;
	} while(1);
}




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

void cprintf(char* fmt, char* s)
{
  static char temp[64];
  sprintf(temp, fmt, s);
  cputs(temp);
}
