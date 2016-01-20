
#include "GRLIB.h"
#include "raycast.h"
#include "keyboard.h"
#include <CONIO.h>
#include <STDIO.h>
#include <MATH.h>
#include <ALLOC.H>

unsigned char  *Wall;
unsigned char  *Wall1;
unsigned char far *Fondo;

unsigned char far *Source; // Pointer for the ShowPage32 proc
unsigned char far *Dest;

extern far ShowPage32();


int LoadImage(char far* SCREEN, char *path)
{
   FILE    *stream;
   long    j;
   int     c;
   unsigned int Height,Width;

   if ((stream = fopen(path,"rb")) == NULL)
      return 1;
   else
   {
      fgetc(stream);
      fgetc(stream);
      Width = fgetc(stream);
      Width += fgetc(stream) << 8;
      Height = fgetc(stream);
      Height += fgetc(stream) << 8;

      fseek (stream,32,SEEK_SET);  // Nos brincamos el header
      for (j=0;j<256;j++)          // Leemos la paleta
      {
         c = fgetc(stream);
         Palette[j].R = c;
         c = fgetc(stream);
         Palette[j].G = c;
         c = fgetc(stream);
         Palette[j].B = c;
      }
      SetActPalette(Palette);              // La activamos
      j = 0;
      while (j<Width*Height)
      {
         c = getc(stream);
         SCREEN[j++] = (byte)c;     // Leemos el bitmap directo
      }                             // en la pantalla
      fclose(stream);
      return 0;
   }
}



void main(void)
{
   int j,k;
   long xp=6*CELL_WIDTH,
       yp=6*CELL_HEIGHT,dx,dy,
       angle = 500;
   char key;
   float rad;

   clrscr();
   SetMode();
   LoadMap("RayMap.Dat");
   Wall    = (unsigned char *)malloc(64*64);
   Wall1   = (unsigned char *)malloc(64*64);
   ClearScreen(SCREEN);
   MakeTables();
      // get mem for double buffer.
   VSCREEN = (char far*)farmalloc(64000);
   Fondo   = (char far*)farmalloc(64000);
   LoadImage(Fondo,"fondo.cel");
   LoadImage(Wall,"walltext.cel");
   LoadImage(Wall1,"wall1.cel");
   InstallKeyboardHandler();
   while (!keypress[ESC])
   {
      Dest = VSCREEN;
      Source = Fondo;
      ShowPage32();
      RayCast(xp,yp,angle);
      Dest = SCREEN;
      Source = VSCREEN;
      ShowPage32();
      dx=dy=0;
      if (keypress[RIGHT])
         if ((angle += WorldAngles[6]) > WorldAngles[360])
            angle = WorldAngles[0];
      if (keypress[LEFT])
         if ((angle -=WorldAngles[6]) < WorldAngles[0])
            angle = WorldAngles[360];

      if (keypress[UP])
      {
         rad = 6.28*angle/WorldAngles[360];
         dx=cos(rad)*30;
         dy=sin(rad)*30;
      }

      if (keypress[DOWN])
      {
         rad = 6.28*angle/WorldAngles[360];
         dx=-cos(rad)*30;
         dy=-sin(rad)*30;
      }
  // move player

  xp+=dx;
  yp+=dy;

   }
   RemoveKeyboardHandler();

   SetText();

   farfree(VSCREEN);
   farfree(Fondo);
   FreeTables();
//   if (Wall != NULL)
      free(Wall);
      free(Wall1);
//   farfree(Wall1);


}