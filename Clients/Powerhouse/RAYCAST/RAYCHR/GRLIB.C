/* ----------------------------------------------------------------------- */
/*  GRLIB.C              (C) Copyright                           
 *
 * Description:
 * 	Graphics functions
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

#include <MEM.h>
#include <CONIO.h>
/* ----------------------------------------------------------------------- */
/* Third part include files */


/* ----------------------------------------------------------------------- */
/* Libraries include files */


/* ----------------------------------------------------------------------- */
/* Project include files */

#include "GRLIB.h"

/* ----------------------------------------------------------------------- */
/* Local constants & definitions declaration */


/* ----------------------------------------------------------------------- */
/* Global variables declaration */

   byte far *SCREEN = (char far *)0xA0000000L;
   tPalette   Palette;

/* ----------------------------------------------------------------------- */
/* Local variables declaration (static) */


/* ----------------------------------------------------------------------- */
/* Local functions prototypes (static) */

void SetMode(void);
void SetText(void);
void WaitRetrace(void);
void ClearScreen(char far *SCREEN);
void PutPixel(char far *SCREEN,int X, int Y, byte Color);
byte GetPixel(char far *SCREEN, int X, int Y);
tColor GetPal(byte ColorNo);
void SetColor(byte ColorNo,tColor C);
void SetActPalette(tPalette Palette);
void ShowPage(unsigned char far *dest, unsigned char far *source);


/* ----------------------------------------------------------------------- */
/* Functions code */


/* ----------------------------------------------------------------------- */
// This function sets de graphic mode
/* ----------------------------------------------------------------------- */

void SetMode( void)
{
   asm {
      mov ah,0x00
      mov al,0x13
      int 0x10
   }
} /* SetMode */

/* ----------------------------------------------------------------------- */
// This function switch to the text mode
/* ----------------------------------------------------------------------- */

void SetText(void)
{
   asm  {
      mov ah,0x00
      mov al,0x03
      int 0x10
   }
}

/* ----------------------------------------------------------------------- */
// This function waits for the vertical retrace of the monitor and then 
// exits
/* ----------------------------------------------------------------------- */

void WaitRetrace(void)
{
      asm mov dx,0x3da
   l1:
      asm in al,dx
      asm and al,0x08
      asm jnz l1
   l2:
      asm in al,dx
      asm and al,0x08
      asm jz l2

}

/* ----------------------------------------------------------------------- */
// This function clears the next 64000 bytes starting at the pointer SCREEN
/* ----------------------------------------------------------------------- */
void ClearScreen(char far *SCREEN)
{
   _fmemset(SCREEN,0,64000);
}


/* ----------------------------------------------------------------------- */
// Draw a pixel with the color Color in X,Y (Mode 320x200)
/* ----------------------------------------------------------------------- */

void PutPixel(char far *SCREEN,int X, int Y, byte Color)
{
   SCREEN [(Y << 8) + (Y << 6) + X] = Color;
}

/* ----------------------------------------------------------------------- */
// returns the color of the pixel in X,Y (Mode 320x200)
/* ----------------------------------------------------------------------- */

byte GetPixel(char far *SCREEN, int X, int Y)
{
   return (SCREEN [(Y << 8) + (Y << 6) + X]);
}

/* ----------------------------------------------------------------------- */
// returns the RGB values of in the VGA palette at the index ColorNo
/* ----------------------------------------------------------------------- */

tColor GetPal(byte ColorNo)
{
   tColor C;
   outp( 0x3C8, ColorNo);
   C.R = inp( 0x3C9);
   C.G = inp( 0x3C9);
   C.B = inp( 0x3C9);
   return (C);
}

/* ----------------------------------------------------------------------- */
// Sets the RGB values in the VGA palette at the index ColorNo
/* ----------------------------------------------------------------------- */

void SetColor(byte ColorNo,tColor C)
{
   outp( 0x3C8, ColorNo);
   outp( 0x3C9, C.R);
   outp( 0x3C9, C.G);
   outp( 0x3C9, C.B);
}
/* ----------------------------------------------------------------------- */
// Change all the VGA palette for Palette
/* ----------------------------------------------------------------------- */

void SetActPalette(tPalette Palette)
{
   int j;
   WaitRetrace();
   for (j=0;j<256;j++)
      SetColor(j,Palette[j]);

}

/* ----------------------------------------------------------------------- */
//  Copies 64000 bytes from source to dest
/* ----------------------------------------------------------------------- */

void ShowPage(unsigned char far *dest, unsigned char far *source)
{
   _fmemcpy(dest,source,64000);
/*
   asm {
      push  ds
      cld
      les   di,[dest]
      lds   si,[source]
      mov   cx,32000
      rep   movsw
      pop   ds
   }
   */
}



/* ----------------------------------------------------------------------- */
/* End of GRLIB.C */
