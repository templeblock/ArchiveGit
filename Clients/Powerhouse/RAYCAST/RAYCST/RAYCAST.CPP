#include <dos.h>
#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <alloc.h>
#include <bios.h>
#include "video.h"

#define RAYLENGTH 60
#define MAXLENGTH 220
#define WORLDX 256
#define WORLDY 256
#define WORLDSIZE 65535
// The altitude of the helicopter. (Above ground)
#define USERALT 16
#define BUFSEG FP_SEG(Buffer)
#define BUFOFS FP_OFF(Buffer);

struct RGB {
	byte red;
	byte green;
	byte blue;
};

unsigned far char* World;
unsigned far char* Color;
unsigned far char* Sky;
unsigned far char* Cockpit;
int sine[1280];
int cosine[1280];
int startpostable [MAXLENGTH+50];
char far* Buffer;
int USERX;
int USERY;
int USERA;

void DrawSky (int Angle,int l)
// Draw the sky.
{
  unsigned int BufSeg=BUFSEG;
  unsigned int SkySeg=FP_SEG (Sky);
  asm{
	 push es
	 push di
	 push ds
	 push si

	 mov ax,SkySeg // Set DS:SI to the sky picture
	 mov ds,ax
	 mov ax,Angle
	 add ax,4
	 mov si,ax

	 mov ax,BufSeg // Set ES:DI to the Screen buffer
	 mov es,ax
	 mov ax,l
	 mov di,ax

	 mov cx,200 // The number of rows

	 CopyLoop:
	 mov al,[ds:si]
	 mov [es:di],al
	 add di,320
	 add si,320
	 dec cx
	 jnz CopyLoop

	 pop si
	 pop ds
	 pop di
	 pop es
  };
};

void DrawIt(int startpos, int r, int xpos, int OldPos, unsigned char c)
{
	unsigned int BufSeg = FP_SEG (Buffer);
	if ((startpos < 200) && (OldPos <200))
	{
	asm{
		push es
		push di

		mov ax,r             // <---- Here
		cmp ax,10
		ja  IsBigger

		// If r is less than 25, that is, the ray is closer than 25 units
		// from the viewer, use this method.
		// The reason why I use this method is that you'll get transparent
		// ground otherwise.
		mov ax,BufSeg
		mov es,ax

		mov ax,63680
		add ax,xpos
		mov di,ax

		mov cx,200  // Set CX to the number of pixels to be written
		sub cx,OldPos //startpos
		jmp near Draw
		// End of r<=25

		//If r is greater than 25....
		IsBigger:                   // <---- And here
		mov ax,startpos            // See if we need to draw.
		cmp ax,OldPos
		jng NoDraw   // No. Skip this position.
		mov ax,BufSeg // Set ES:DI to the lowest pixel in the column (StartPos)
		mov es,ax

		mov ax,startpos
		mov dx,320
		mul dx
		add ax,xpos
		mov di,ax

		mov cx,startpos  // Sätt CX to the number of pixels to be written
		sub cx,OldPos
		// End of r>25

		Draw: // Draw the column

		mov al,c  //Set AL to the color of the pixel

		DrawLoop:     // And draw it.
		mov [es:di],al
		sub di,320
		dec cx
		jnz DrawLoop

		NoDraw:
		pop di
		pop es
	 };
  };
};

void DrawLine (int xpos)
// This routine draws column xpos on the screen. Xpos ranges from 0 to 319, and
// the whole screen is a quarter of a full circle (1280 degrees in THIS system).
// Using Sine and Cosine values and then interpolating
// between them must be the slowest possible method available.
// Feel free to optimize.
{
  int Alt=(World[(USERY<<8)+USERX]+USERALT); // Get the altitude of the viewer
  int OldPos=199; // Used to speed up drawing
  int Angle=(xpos+USERA)%1280; // The angle of the column
  DrawSky (Angle%320,xpos); // Draw the sky.

  int x1=(USERX+(cosine [Angle]));//%WORLDX;
  int y1=(USERY-(sine [Angle]));//%WORLDY;
  int x2=USERX;
  int y2=USERY;

  int y_unit,x_unit; // Variables for amount of change
							//  in x and y

  unsigned int offset=y1*256+x1; // Calculate offset into video RAM

  int ydiff=y2-y1;   // Calculate difference between
							//  y coordinates
  if (ydiff<0) // If the line moves in the negative direction
  {
	 ydiff=-ydiff;    // ...get absolute value of difference
	 y_unit=-256;     // ...and set negative unit in y dimension
  }
  else y_unit=256;   // Else set positive unit in y dimension

  int xdiff=x2-x1;	// Calculate difference between x coordinates
  if (xdiff<0)       // If the line moves in the negative direction
  {
	  xdiff=-xdiff;	// ...get absolute value of difference
	  x_unit=-1;		// ...and set negative unit in x dimension
  }
  else x_unit=1;	  	// Else set positive unit in y dimension

  int error_term=0;			// Initialize error term
  if (xdiff>ydiff) // If difference is bigger in x dimension
  {
	 int length=xdiff+1;	// ...prepare to count off in x direction
	 for (int i=length; i>0; i--)// Loop through points in x direction
	 {
		offset+=x_unit;				// Move offset to next pixel in x direction
		error_term+=ydiff;		// Check to see if move required in y direction
		if (error_term>xdiff)
		{	// If so...
		  error_term-=xdiff;		// ...reset error term
		  offset+=y_unit;				// ...and move offset to next pixel in y dir.
		};
		// Get the color for the point (x,y) and the right altitude for it.
		byte c=Color [offset];
		long height = ((World [offset]-Alt)<<4)/i;

		// Perspective transform
		int startpos = startpostable [i];
		startpos -= height;

		// OK, draw it!
		DrawIt (startpos,i,xpos,OldPos,c);
		if (i==26)
		  DrawIt (startpos,25,xpos,OldPos,c);

		OldPos = startpos;  // Used to speed up rendering.
	 };
  }
  else
  {								// If difference is bigger in
								//  y dimension
	 int length=ydiff+1;	// ...prepare to count off in
								//  y direction
	 for (int i=length; i>0; i--)
	 {	// Loop through points in y direction
		offset+=y_unit;				// Move offset to next pixel
									 //  in y direction
		error_term+=xdiff;    // Check to see if move
									 //  required in x direction
		if (error_term>0)
		{		// If so...
		  error_term-=ydiff;	// ...reset error term
		  offset+=x_unit;			// ...and move offset to next pixel in x dir.
		};
		// Get the color for the point (x,y) and the right altitude for it.
		byte c=Color [offset];
		long height = ((World [offset]-Alt)<<4)/i;

		// Perspective transform
		int startpos = startpostable [i];
		startpos -= height;

		// OK, draw it!
		DrawIt (startpos,i,xpos,OldPos,c);
		if (i==26)
		  DrawIt (startpos,25,xpos,OldPos,c);

		OldPos = startpos;  // Used to speed up rendering.
	 };
  };
};

void InitSinCos(void)
// Setup Sine and cosine tables.
{
	printf ("InitSinCos :");
	for (int a=0;a<1280;a++)
	{
		float A=a;
		A=A/(320/0.5); // A divided by 320/FOV in radians
		A=A*M_PI;
		sine [a]=sin (A)*100;
		cosine [a]=cos (A)*100;
		if ((a%100)==0) printf (">");
	};
};

void InitStartPosTable(void)
// Speed up prespective transformations by using lookup tables.
{
	printf ("\nInit Startpos :");
	for (int i=1;i<(RAYLENGTH+50);i++)
	{
		startpostable [i] =500/i+80;
		// The number 80 above is the y position of the horizon.
		// To simulate tilting the helicopter forward, decrease it to 20 or 40
		// increase it to tilt backwards.
		if ((i%10)==0) printf (">");
	};
};

void DupCols(void)
// If you only draw every other column, duplicate the columns here.
{
	unsigned int BufSeg=BUFSEG;
	asm{
		push es
		push di
		push ds
		push si

		mov ax,BufSeg // DS:SI = Buffer
		mov ds,ax
		mov ax,0
		mov si,ax

		mov ax,BufSeg // ES:DI = Buffern+1
		mov es,ax
		mov ax,1
		mov di,ax

		mov cx,32000 // Set CX to the number of WORDS in the Buffer

		CopyLoop:
		mov al,[ds:si]
		add si,2
		mov [es:di],al
		add di,2
		dec cx
		jnz CopyLoop

		pop si
		pop ds
		pop di
		pop es
	};
};

void BlitBuffer (void)
// Move the buffer to the screen at A000:0000.
{
	unsigned int BufSeg=BUFSEG;
	asm{
		push es
		push di
		push ds
		push si

		mov ax,BufSeg // Set DS:SI to the buffer
		mov ds,ax
		mov ax,0
		mov si,ax

		mov ax,0xA000 // Set ES:DI to the screen
		mov es,ax
		mov ax,0
		mov di,ax

		mov cx,32000 // Set cx to the number of WORDS

		cld
		rep movsw

		pop si
		pop ds
		pop di
		pop es
	};
};

void DrawCockpit(void)
// Blit the instrument panel image.
{
	unsigned int BufSeg=BUFSEG;
	unsigned int CockpitSeg=FP_SEG (Cockpit);
	asm{
		push es
		push di
		push ds
		push si

		mov ax,CockpitSeg // Set DS:SI to pint to the instrument panel image
		mov ds,ax
		mov ax,8
		mov si,ax

		mov ax,BufSeg // Set ES:DI to point to the Buffer
		mov es,ax
		mov ax,0
		mov di,ax

		mov cx,63999 // Set cx to the number of BYTES

		cld

		DrawLoop:
		lodsb
		cmp al,9
		jz Transparent
		stosb
		jmp near Done
		Transparent:
		inc di
		Done:
		dec cx
		jnz DrawLoop

		lodsb
		stosb

		pop si
		pop ds
		pop di
		pop es
	};
};

#pragma argsused
void main (int argc, char *argv[])
{
	InitSinCos();
	InitStartPosTable();
	Buffer = (byte far*) farmalloc (64000);

	printf ("\nReading data files....");

	FILE *Data = fopen ("ALTITUDE.BMP","rb");
	World = (unsigned char far*) farmalloc (WORLDSIZE);
	fseek (Data,1078,SEEK_SET);
	fread ((void*)World,WORLDSIZE,1,Data);
	fclose (Data);

	Data=fopen ("COLOR.BMP","rb");
	Color = (unsigned char far*) farmalloc (WORLDSIZE);
	fseek (Data,1078,SEEK_SET);
	fread ((void*)Color,WORLDSIZE,1,Data);
	fclose (Data);

	Data=fopen ("SKY.BMP","rb");
	Sky = (unsigned char far*) farmalloc (64000);
	fseek (Data,1078,SEEK_SET);
	fread ((void*)Sky,64000,1,Data);
	fclose (Data);

	Data=fopen ("CPIT.BMP","rb");
	Cockpit = (unsigned char far*) farmalloc (64000);
	fseek (Data,1078,SEEK_SET);
	fread ((void*)Cockpit,64000,1,Data);
	fclose (Data);

	VidOn();

	palLoad ("RAYCAST.COL");
	palSet();

	USERX=100; // Starting position
	USERY=100;
	USERA=960; // Starting angle.

	long starttime=biostime (0,0);
	int frames=0;

/* The program currently draws *every* column.
	To speed things up (Comanche "big pixels"), implement the changes. */
	while (!kbhit())
	{
		for (int l=0;l<320;l++)  // Replace l++ with l+=2.
		{
			DrawLine (l);
		};
//		DupCols();      // Remove the comment sign ("//")
		DrawCockpit();
		BlitBuffer();
		USERX++;
		USERY++;
//		USERA++;        // UnComment this line to rotate.
//		USERA%=1280;    // If you rotate, you need this line.
		USERX=USERX%WORLDX;
		USERY=USERY%WORLDY;
		frames++;
	};
	starttime=biostime (0,0)-starttime;

	getch();
	farfree ((void *)Color);
	farfree ((void *)World);
	farfree ((void *)Buffer);
	farfree ((void *)Sky);
	farfree ((void *)Cockpit);
	VidOff();
	int FPS=(frames*10)/((starttime*10)/182);
	int AFT=((starttime*1000)/182)/frames;
	// FPS if frames per second times 10, so a FPS of 27 means 2.7 frames per second.
	// AFT is the average frame time in 1/100s of a second.
	printf ("FPS=%d    AFT=%d/100\nPress any key....",FPS,AFT);
	getch();
};
