/* ----------------------------------------------------------------------- */
/* raycast.c      (C) Copyright
 *
 * Description:
 * 	ray-casting engine
 *
 * This module belongs to the project:
 *
 *
 * This module may be compiled by: 	'C compiler'
 *
 * Libraries:
 *     GRLIB      // Graphic functions
 *
 * Programmer: Jaime del Palacio CIS : 73072,3134
 *
 */


/* ----------------------------------------------------------------------- */
/* Remarks */

/* ----------------------------------------------------------------------- */
/* Conditional defines */

/* ----------------------------------------------------------------------- */
/* Compiler include files */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <alloc.h>
#include <mem.h>

/* ----------------------------------------------------------------------- */
/* Third part include files */


/* ----------------------------------------------------------------------- */
/* Libraries include files */


/* ----------------------------------------------------------------------- */
/* Project include files */

#include "GRLIB.h"       // Graphic functions for mode 13
#include "RAYCAST.h"     // constants
#include "ERRORS.h"      // Error number constants
#include "FIXED.h"       // Fixed point aritmethics

/* ----------------------------------------------------------------------- */
/* Local constants & definitions declaration */


/* ----------------------------------------------------------------------- */
/* Global variables declaration */

unsigned char far *VSCREEN = NULL;

byte  Map[MAP_WIDTH][MAP_HEIGHT];      // World 2d Map
float far* Tan;     // Tangent Look-up Table
float far* InvTan;  // 1/tangent look up tbl
unsigned int * WorldAngles;   // World arcs -> 360 deg look up tbl
float * PreLineX;
float * PreLineY;
float far* InvSin;
float far* InvCos;
float far* Cos;


extern unsigned char *Wall;
extern unsigned char *Wall1;

/* ----------------------------------------------------------------------- */
/* Local variables declaration (static) */


/* ----------------------------------------------------------------------- */
/* functions prototypes */

err LoadMap(char *file);
void RayCast(int x, int y, long angle);
void MakeTables(void);
void Mapper (unsigned char far *SCREEN,unsigned char *Tx, long dstx,long dsty, long txcol, long run);
// temp functions

void vline(unsigned char far *SCREEN,int x1,int y1, int y2,byte Color);


/* ----------------------------------------------------------------------- */
/* Functions code */

/* ----------------------------------------------------------------------- */
// LoadMap : Loads the world map into Map array
// the Map file most be WORLD_WIDTH x WORLD_HEIGHT
// !!!! It needs to have more than 0..9 object types.
/* ----------------------------------------------------------------------- */
err LoadMap(char *file)
{
   FILE   *Level;
   int    obj,k,j;

   if ((Level = fopen(file,"rt")) == NULL)
      return (ERR_ERROR_OPENING_FILE);
   else
   {
      for (k=0; k <= MAP_HEIGHT; k ++)
      {
         for (j=0; j < MAP_WIDTH; j++)
         {
            obj = getc(Level);
            if (obj > 48)
               Map[j][k] = obj-48;   // Convert ascii to integer
            if (Map[j][k] == EOF)
            {
               fclose(Level);
               return (ERR_ERROR_IN_MAP_FILE);
            }
         }
         getc(Level);   // Drop the CR char #10
      }
      fclose(Level);
   }
   return (ERR_NO_ERROR);
} // LoadMap

/* ----------------------------------------------------------------------- */
// Calculates the look-up tables.
/* ----------------------------------------------------------------------- */
void MakeTables(void)
{
   long j,Arcs=0;
   float Rad,a;
   long EdgeAngle;

   Arcs        = (long)SCREEN_WIDTH * 360 /  FOV;

   Tan         = (float  far*)farmalloc((Arcs+1)*sizeof(float)  );
   Cos         = (float  far*)farmalloc((Arcs+1)*sizeof(float)  );
   InvTan      = (float  far*)farmalloc((Arcs+1)*sizeof(float)  );
   InvSin      = (float  far*)farmalloc((Arcs+1)*sizeof(float)  );
   InvCos      = (float  far*)farmalloc((Arcs+1)*sizeof(float)  );
   PreLineX    = (float *)malloc((Arcs+1)*sizeof(float));
   PreLineY    = (float *)malloc((Arcs+1)*sizeof(float));
   WorldAngles = (unsigned int *)malloc ((360+1) * sizeof(unsigned int));

      // Makes the Deg. to World Angles trans look up

   for (j=0; j <= 360; j++)
      WorldAngles[j] = (long)j*((float)Arcs/360);

      // Make slope lookup table

   for (j=0 ; j <= Arcs; j++)
   {
         // Deg to Rad with a little add so the tan never
         // gets to 90 deg or 180 deg
      Rad = (float)((3.272e-4) + j * 2*3.141592654/Arcs);
      Tan[j]    = (float) tan (Rad);
      InvTan[j] = (float) (1/Tan[j]);


         // Correct the sign of the tangent

      if (j>=WorldAngles[0] && j<WorldAngles[180])
         PreLineY[j] = (float)(fabs(Tan[j]  * CELL_HEIGHT));
      else
         PreLineY[j] = (float)(-fabs(Tan[j]  * CELL_HEIGHT));

      if (j>=WorldAngles[90] && j<WorldAngles[270])
         PreLineX[j] = (float)(-fabs(InvTan[j]  * CELL_WIDTH));
      else
         PreLineX[j] = (float)(fabs(InvTan[j]  * CELL_WIDTH));

      InvCos[j] = (float)(1/cos(Rad));
      InvSin[j] = (float)(1/sin(Rad));

   } // for all arcs

   EdgeAngle = WorldAngles[FOV >> 1];
   for (j = -EdgeAngle; j <= EdgeAngle; j++)
   {
      Rad = (float)((3.272e-4) + j * 2*3.141592654/WorldAngles[360]);
      Cos[j+EdgeAngle] = (208*64/cos(Rad));
   }


} // MakeTables



void FreeTables (void)
{
   farfree(Tan);
   farfree(Cos);
   farfree(InvTan);
   farfree(InvSin);
   farfree(InvCos);
   free(PreLineX);
   free(PreLineY);
   free(WorldAngles);

}


/* ----------------------------------------------------------------------- */
// Raycasting engine function : renders the view-window from x,y viewing
// in direction angle
/* ----------------------------------------------------------------------- */
void RayCast(int x, int y, long angle)
{
   signed int Color,j,k,
        LookForX,LookForY,  // Where is the next cell acrording to the angle
        DeltaX,DeltaY,
        nix,niy,            // Next interesection.
        top,bottom,

   // Pre coputed data for the first interesection
        LookForX1,LookForY1,  // Where is the next cell acrording to the angle
        DeltaX1,DeltaY1,
        nix1,niy1,            // Next interesection.
        LookForX2,LookForY2,  // Where is the next cell acrording to the angle
        DeltaX2,DeltaY2,
        nix2,niy2            // Next interesection.
        ;
   signed long  CurrAngle,  // Current Angle being casted
        CurrCellX,CurrCellY
                ;

   byte ObjX,ObjY;
   int  Casting;
   float  xi_save,yi_save;
   float  xi,yi,xi1,xi2,yi1,yi2;
   long Scale;
   float xdist,ydist;

   CurrAngle = (long)angle - WorldAngles[FOV >> 1];
   if (CurrAngle < WorldAngles[0])
      CurrAngle += WorldAngles[360];

      // Make some precomputed data for 1st interection
   niy1      = CELL_HEIGHT + (y & AND_CELL_HEIGHT);
   niy2      = (int)(y & AND_CELL_HEIGHT);
   nix1      = CELL_WIDTH + (x & AND_CELL_WIDTH);
   nix2      = (int)(x & AND_CELL_WIDTH);



   for (j=0; j < VIEW_X_SIZE ; j++)   // Start ray-casting for all the screen
   {
         // Look for the 1st interesction
      if (CurrAngle >= WorldAngles[0] && CurrAngle < WorldAngles[180])
      {
         niy      = niy1;
         xi       = InvTan[CurrAngle] * (niy - y) + x;
         LookForY = 0;
         DeltaY   = CELL_HEIGHT;
      }
      else
      {
         niy      = niy2;
         xi       = InvTan[CurrAngle] * (niy - y) + x;
         LookForY = -1;
         DeltaY   = -CELL_HEIGHT;
      }

      if (CurrAngle < WorldAngles[90] || CurrAngle >= WorldAngles[270])
      {
         nix      = nix1;
         yi       = Tan[CurrAngle] * (nix - x) + y;
         LookForX = 0;
         DeltaX   = CELL_WIDTH;
      }
      else
      {
         nix      = nix2;
         yi       = Tan[CurrAngle] * (nix - x) + y;
         LookForX = -1;
         DeltaX   = -CELL_WIDTH;
      }

      ObjX = ObjY = 0;
      Casting = 2;

         // *** Cast the ray
         // inner loop

      while (ObjX == 0)
      {
         CurrCellX = (nix+LookForX) >> SHIFT_CELL_WIDTH;
         CurrCellY = (int)(yi) >> SHIFT_CELL_HEIGHT;
         if ((ObjX = Map[CurrCellY][CurrCellX]) != 0)
         {
            xdist = (yi-y)*InvSin[CurrAngle];
            yi_save = yi;
         }
         else
            yi += PreLineY[CurrAngle];
         nix += DeltaX;

      }
      while (ObjY == 0)
      {
         CurrCellX = (int)(xi) >> SHIFT_CELL_WIDTH;
         CurrCellY = (niy+LookForY)  >> SHIFT_CELL_HEIGHT;
         if ((ObjY = Map[CurrCellY][CurrCellX]) != 0)
         {
            ydist = (xi-x)*InvCos[CurrAngle];
            xi_save = xi;
         }
         else
            xi += PreLineX[CurrAngle];
         niy += DeltaY;
      }  // while
      // Draw line of texture

      if (xdist < ydist)
      {
         if (xdist == 0)
            xdist = 1e-10;
         Scale = (long)(Cos[j] /xdist);
         if ((top = VIEW_Y_HALF - (Scale >>1)) < VIEW_Y_START)
            top = VIEW_Y_START;

         Mapper(VSCREEN,Wall,j+VIEW_X_START,top,(long)yi_save%CELL_WIDTH,Scale);
      }   // xdist < ydist
      else
      {
         if (ydist == 0)
            ydist = 1e-10;
         Scale = (long)(Cos[j] / ydist);
         if ((top = VIEW_Y_HALF - (Scale >> 1)) < VIEW_Y_START)
            top = VIEW_Y_START;

         Mapper(VSCREEN,Wall1,j+VIEW_X_START,top,(long)xi_save%CELL_WIDTH,(int)Scale);
      }   // xdist >= ydist


      if(++CurrAngle >= WorldAngles[360])
         CurrAngle = WorldAngles[0];

   }  // main for


} // RayCast


/* ----------------------------------------------------------------------- */
// Temp Function: Vertical Line on X1,Y1 to X1,Y2 (Mode 320x200)
/* ----------------------------------------------------------------------- */

void vline(unsigned char far *SCREEN,int x1,int y1, int y2,byte Color)
{
asm  {
      les  di,SCREEN
      mov di,y1
      sub y2,di

      shl di,6
      mov bx,di
      shl di,2
      add di,bx
      add di,x1
      mov al,Color
     }
Loop:
asm  {
      mov es:[di],al
      add di,320
      dec y2
      cmp y2,0
      jnz Loop
     }
} // VLine

void Mapper (unsigned char far *SCREEN,unsigned char *Tx,
            long dstx,long dsty, long txcol, long run)
{
   register unsigned int dindex,j;
   unsigned int index = txcol;
   float scale,step,sindex=0,top;
   int dif,shade;


   top = index;
   if (run == 0)
      run = 1e-10;
   step  = (float)CELL_WIDTH/run;
   dif = VIEW_Y_SIZE-(run);
   if (dif < 0)
      sindex = abs(dif>>1) * step;
   if (run > VIEW_Y_SIZE)
      run = VIEW_Y_SIZE;
   dindex    = (dsty << 8) + (dsty << 6) + dstx;
   if( (shade = (32 * (int)(64/run))) > 160)
      shade = 160;

   for (j=0; j<run; j++)
   {
      SCREEN[dindex] = Tx[index] + shade;
      asm    ADD [dindex],320
      sindex += step;
      index = (long)(txcol + ((int)sindex << SHIFT_CELL_WIDTH));
   }
}





/* ----------------------------------------------------------------------- */
/* End of raycast.c */
