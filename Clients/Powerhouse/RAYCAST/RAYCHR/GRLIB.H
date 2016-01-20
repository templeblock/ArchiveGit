/* ----------------------------------------------------------------------- */
/* grlib.h        (C) Copyright 
 *
 * Description:
 * 	Header file for the graphic functions
 * 	
 * This module belongs to the project:
 * 	CRON
 * 
 * This module may be compiled by: 	'C compiler'
 *
 * Libraries: 
 * 	
 *
 * Programmer: Jaime del Palacio
 *
 */


/* ----------------------------------------------------------------------- */
/* Remarks */

/* ----------------------------------------------------------------------- */
/* Conditional defines */


/* ----------------------------------------------------------------------- */
/* Compiler include files */


/* ----------------------------------------------------------------------- */
/* Third part include files */


/* ----------------------------------------------------------------------- */
/* Libraries include files */


/* ----------------------------------------------------------------------- */
/* Project include files */


/* ----------------------------------------------------------------------- */
/* Global constants & definitions declaration */

#define byte unsigned char
#define SCREEN_WIDTH    320      // Screen resolution
#define SCREEN_HEIGHT   200

/* ----------------------------------------------------------------------- */
/* Structures */

typedef struct
        {
           byte R,G,B;
        }tColor;

typedef tColor tPalette[256];


/* ----------------------------------------------------------------------- */
/* External global variables */

extern unsigned char far *SCREEN;
extern tPalette    Palette;



/* ----------------------------------------------------------------------- */
/* Global functions & clases declaration */

extern void SetMode(void);
extern void SetText(void);
extern void WaitRetrace(void);
extern void ClearScreen(char far *SCREEN);
extern void PutPixel(char far *SCREEN,int X, int Y, byte Color);
extern byte GetPixel(char far *SCREEN,int X, int Y);
extern tColor GetPal(byte ColorNo);
extern void SetColor(byte ColorNo,tColor C);
extern void SetActPalette(tPalette Palette);
extern void ShowPage(unsigned char far *dest, unsigned char far *source);


/* ----------------------------------------------------------------------- */
/* End of grlib.h */
