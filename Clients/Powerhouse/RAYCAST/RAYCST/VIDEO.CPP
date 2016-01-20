#include <stdio.h>
#include <dos.h>
#include "video.h"

struct RGB
{
  byte Red;
  byte Green;
  byte Blue;
};

RGB palPalette[256];

void palLoad (char *Name)
{
  FILE* Palette=fopen (Name,"rb");
  fread (palPalette,1,768,Palette);
  fclose (Palette);
};

void palSet(void)
{
  palSetDAC ((char *)palPalette);
};

void palSetDAC(char *Palette)
{
  union REGS r;
  struct SREGS s;

  s.es = FP_SEG(Palette);       //dac_block is a pointer to DAC structure
  r.x.dx = FP_OFF(Palette);

  r.x.ax = 0x1012;      //Function 1012 is set block of DAC registers
  r.x.bx = 0;   //BX holds number to begin with (0 to 255)
  r.x.cx = 255; //CX holds how many to set
  int86x(0x10, &r, &r, &s);

  return;
};

void VidOn (void)
{
  asm{
	 mov ax,0x13
	 int 0x10
  };
};

void VidOff (void)
{
  asm{
	 mov ax,0x3
	 int 0x10
  };
};
